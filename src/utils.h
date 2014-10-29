#ifndef DUV_UTILS_H
#define DUV_UTILS_H

#include "duv.h"

duk_bool_t duv_is_handle(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_req(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_fs(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_stream(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_timer(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_tcp(duk_context* ctx, duk_idx_t index);
uv_loop_t* duv_loop(duk_context *ctx);
duv_handle_t* duv_setup_handle(duk_context *ctx);
duv_handle_t* duv_cleanup_handle(duk_context *ctx, duv_handle_t *data);
duv_req_t* duv_setup_req(duk_context *ctx, int callback_index);
duv_req_t* duv_cleanup_req(duk_context *ctx, duv_req_t *data);
void duv_error(duk_context *ctx, int status);
int duv_push_status(duk_context *ctx, int status);
void duv_check(duk_context *ctx, int status);
void duv_store_handler(duk_context *ctx, duv_handle_t *data, int type, int index);
void duv_emit_event(duk_context *ctx, duv_handle_t* data, duv_callback_id type, int nargs);
void duv_fulfill_req(duk_context *ctx, uv_req_t* req, int nargs);
void duv_get_data(duk_context *ctx, int index, uv_buf_t* buf);
const char* duv_protocol_to_string(int family);

#endif
