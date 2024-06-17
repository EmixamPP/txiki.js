#include "blob_store.h"

#include "hash.h"
#include "private.h"

#include <stdio.h>
#include <time.h>

static TJSObjectURL *BLOBS_STORE = NULL;

static void generate_random_url(char *dest) {
    strcpy(dest, "blob:");
    // 5 is the length of "blob:"
    for (size_t i = 5; i < OBJECT_URL_SIZE; i++) {
        size_t rindex = (double) rand() / RAND_MAX * (sizeof OBJECT_URL_CHARSET - 1);
        *(dest + i) = OBJECT_URL_CHARSET[rindex];
    }
    *(dest + OBJECT_URL_SIZE) = '\0';  // null-terminator
}

static void tjs__object_url_free(const TJSObjectURL *obj_url) {
    JS_FreeValue(obj_url->ctx, obj_url->obj);
    HASH_DEL(BLOBS_STORE, obj_url);
}

void tjs__blobstore_init() {
    srand(time(NULL));  // Seed the random number generator
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

const TJSObjectURL *tjs__get_objecturl_object(const char *url) {
    TJSObjectURL *obj_url = NULL;
    HASH_FIND_STR(BLOBS_STORE, url, obj_url);
    return obj_url;
}

bool tjs__is_objecturl_url(const char *url) {
    return strncmp(url, "blob:", 5) == 0;
}

static JSValue tjs_createObjectURL(JSContext *ctx, JSValue this_val, int argc, JSValue *argv) {
    TJSObjectURL *obj_url;

    if (argc < 1) {
        return JS_EXCEPTION;
    }

    obj_url = malloc(sizeof(struct TJSObjectURL));
    if (!obj_url) {
        return JS_EXCEPTION;
    }

    generate_random_url(obj_url->url);
    obj_url->obj = JS_DupValue(ctx, argv[0]);
    obj_url->ctx = ctx;

    HASH_ADD_STR(BLOBS_STORE, url, obj_url);


    tjs__get_objecturl_object(obj_url->url);

    return JS_NewString(ctx, obj_url->url);
}


static JSValue tjs_revokeObjectURL(JSContext *ctx, JSValue this_val, int argc, JSValue *argv) {
    TJSRuntime *qrt = JS_GetContextOpaque(ctx);
    CHECK_NOT_NULL(qrt);

    if (argc < 1) {
        return JS_EXCEPTION;
    }

    const TJSObjectURL *obj_url = tjs__get_objecturl_object(JS_ToCString(ctx, argv[0]));

    if (obj_url == NULL) {
        return JS_UNDEFINED;
    }


    tjs__object_url_free(obj_url);

    return JS_UNDEFINED;
}

static const JSCFunctionListEntry tjs_blobstore_funcs[] = { TJS_CFUNC_DEF("createObjectURL", 2, tjs_createObjectURL),
                                                            TJS_CFUNC_DEF("revokeObjectURL", 1, tjs_revokeObjectURL) };

void tjs__mod_blobstore_init(JSContext *ctx, JSValue ns) {
    JS_SetPropertyFunctionList(ctx, ns, tjs_blobstore_funcs, countof(tjs_blobstore_funcs));
}
