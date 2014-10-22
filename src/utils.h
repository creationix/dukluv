#ifndef DUV_UTILS_H
#define DUV_UTILS_H

#include "duv.h"

static uv_loop_t* duv_loop(duk_context *ctx);
static void duv_setup_handle(duk_context *ctx, uv_handle_t *handle);
static int duv_error(duk_context *ctx, int status);
static void duv_check(duk_context *ctx, int status);
static void duv_emit_event(duk_context *ctx, uv_handle_t* handle, int type, int nargs);

#endif
