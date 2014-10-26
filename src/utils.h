#ifndef DUV_UTILS_H
#define DUV_UTILS_H

#include "duv.h"

static uv_loop_t* duv_loop(duk_context *ctx);
static duv_handle_t* duv_setup_handle(duk_context *ctx);
static duv_handle_t* duv_cleanup_handle(duk_context *ctx, duv_handle_t *data);
static duv_req_t* duv_setup_req(duk_context *ctx, int callback_index);
static duv_req_t* duv_cleanup_req(duk_context *ctx, duv_req_t *data);
static void duv_error(duk_context *ctx, int status);
static int duv_push_status(duk_context *ctx, int status);
static void duv_check(duk_context *ctx, int status);
static void duv_store_handler(duk_context *ctx, duv_handle_t *data, int type, int index);
static void duv_emit_event(duk_context *ctx, duv_handle_t* data, duv_callback_id type, int nargs);
static void duv_fulfill_req(duk_context *ctx, uv_req_t* req, int nargs);

#endif
