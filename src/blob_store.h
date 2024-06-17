#ifndef TJS_BLOB_STORE_H
#define TJS_BLOB_STORE_H

#include "hash.h"
#include "private.h"

#define OBJECT_URL_SIZE    20
#define OBJECT_URL_CHARSET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

struct TJSObjectURL {
    char url[OBJECT_URL_SIZE];  // hash key
    JSContext *ctx;
    JSValue obj;
    UT_hash_handle hh;  // hash map handle
};

void tjs__blobstore_init();
void tjs__blobstore_destroy();
bool tjs__is_objecturl_url(const char *url);

const TJSObjectURL *tjs__get_objecturl_object(const char *url);

#endif