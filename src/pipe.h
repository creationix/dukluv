#ifndef DUV_PIPE_H
#define DUV_PIPE_H

#include "duv.h"

duk_ret_t duv_new_pipe(duk_context *ctx);
duk_ret_t duv_pipe_open(duk_context *ctx);
duk_ret_t duv_pipe_bind(duk_context *ctx);
duk_ret_t duv_pipe_connect(duk_context *ctx);
duk_ret_t duv_pipe_getsockname(duk_context *ctx);
duk_ret_t duv_pipe_pending_instances(duk_context *ctx);
duk_ret_t duv_pipe_pending_count(duk_context *ctx);
duk_ret_t duv_pipe_pending_type(duk_context *ctx);

#endif
