#include "blob_store.h"

#include "hash.h"
#include "private.h"

static TJSObjectURL *BLOBS_STORE = NULL;

static void tjs__object_url_free(TJSObjectURL *obj_url) {
    HASH_DEL(BLOBS_STORE, obj_url);
    JS_FreeCString(obj_url->ctx, obj_url->url);
    JS_FreeCString(obj_url->ctx, obj_url->blob);
    JS_FreeContext(obj_url->ctx);
    tjs__free(obj_url);
    obj_url = NULL;
}

void tjs__blobstore_destroy() {
    if (BLOBS_STORE == NULL) {
        return;
    }

    TJSObjectURL *tmp;
    TJSObjectURL *obj_url;

    HASH_ITER(hh, BLOBS_STORE, obj_url, tmp) {
        tjs__object_url_free(obj_url);
    }

    BLOBS_STORE = NULL;
}

TJSObjectURL *tjs__get_objecturl_object(const char *url) {
    TJSObjectURL *obj_url = NULL;
    HASH_FIND_STR(BLOBS_STORE, url, obj_url);
    return obj_url;
}

bool tjs__is_objecturl_url(const char *url) {
    return strncmp(url, "blob:", 5) == 0;
}

static JSValue tjs_createObjectURL(JSContext *ctx, JSValue this_val, int argc, JSValue *argv) {
    TJSObjectURL *obj_url;
    JSValue text;
    JSValue text_func;

    if (argc < 2 || !JS_IsObject(argv[0]) || !JS_IsString(argv[1])){
        return JS_EXCEPTION;
    }

    obj_url = tjs__malloc(sizeof(struct TJSObjectURL));
    if (!obj_url) {
        return JS_EXCEPTION;
    }

    text_func = JS_GetPropertyStr(ctx, argv[0], "text");
    if (JS_IsFunction(ctx, text_func)) {
        JSValue text_promise = JS_Call(ctx, text_func, argv[0], 0, NULL);
        int state;

        for (;;) {
            state = JS_PromiseState(ctx, text_promise);
            if (state == JS_PROMISE_FULFILLED) {
                text = JS_PromiseResult(ctx, text_promise);
                break;
            } else if (state == JS_PROMISE_REJECTED) {
                text = JS_Throw(ctx, JS_PromiseResult(ctx, text_promise));
                break;
            } else if (state == JS_PROMISE_PENDING) {
                JSContext *ctx1;
                int err;
                err = JS_ExecutePendingJob(JS_GetRuntime(ctx), &ctx1);
                if (err < 0) {
                    tjs_dump_error(ctx1);
                }
            } else {
                /* not a promise */
                break;
            }
        }
    }

    if (!JS_IsString(text)) {
        tjs__free(obj_url);
        return JS_EXCEPTION;
    }

    obj_url->ctx = ctx;
    obj_url->blob = JS_ToCString(ctx, text);
    obj_url->url = JS_ToCString(ctx, argv[1]);

    HASH_ADD_STR(BLOBS_STORE, url, obj_url);

    return JS_NewString(ctx, obj_url->url);
}

static JSValue tjs_revokeObjectURL(JSContext *ctx, JSValue this_val, int argc, JSValue *argv) {
    TJSRuntime *qrt = JS_GetContextOpaque(ctx);
    CHECK_NOT_NULL(qrt);
    const char *url;

    if (argc < 1) {
        return JS_EXCEPTION;
    }

    url = JS_ToCString(ctx, argv[0]);
    TJSObjectURL *obj_url = tjs__get_objecturl_object(url);

    if (obj_url != NULL) {
        tjs__object_url_free(obj_url);
    }

    JS_FreeCString(ctx, url);
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry tjs_blobstore_funcs[] = { TJS_CFUNC_DEF("createObjectURL", 2, tjs_createObjectURL),
                                                            TJS_CFUNC_DEF("revokeObjectURL", 1, tjs_revokeObjectURL) };

void tjs__mod_blobstore_init(JSContext *ctx, JSValue ns) {
    JS_SetPropertyFunctionList(ctx, ns, tjs_blobstore_funcs, countof(tjs_blobstore_funcs));
}
