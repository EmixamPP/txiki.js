#ifndef TJS_BLOB_STORE_H
#define TJS_BLOB_STORE_H

#include "hash.h"
#include "private.h"

struct TJSObjectURL {
    JSContext *ctx;
    const char *url;  // hash key
    const char *blob;
    UT_hash_handle hh;  // hash map handle
};

void tjs__blobstore_destroy();
bool tjs__is_objecturl_url(const char *url);

TJSObjectURL *tjs__get_objecturl_object(const char *url);

#endif