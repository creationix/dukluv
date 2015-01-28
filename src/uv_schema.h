#ifndef DUV_SCHEMA_H
#define DUV_SCHEMA_H

#include "duv.h"
#include "schema.h"

duk_bool_t duv_is_fd(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_handle(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_req(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_fs(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_stream(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_timer(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_tcp(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_pipe(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_tty(duk_context* ctx, duk_idx_t index);

#endif
