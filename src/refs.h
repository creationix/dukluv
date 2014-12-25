#ifndef DUV_REFS_H
#define DUV_REFS_H

#include "duktape.h"

// Create a global array refs in the heap stash.
void duv_ref_setup(duk_context *ctx);
// like luaL_ref, but assumes storage in "refs" property of heap stash
int duv_ref(duk_context *ctx);
void duv_push_ref(duk_context *ctx, int ref);
void duv_unref(duk_context *ctx, int ref);

#endif
