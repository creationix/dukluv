#ifndef DUV_STREAM_H
#define DUV_STREAM_H

#include "duv.h"

duk_ret_t duv_shutdown(duk_context *ctx);
duk_ret_t duv_listen(duk_context *ctx);
duk_ret_t duv_accept(duk_context *ctx);
duk_ret_t duv_read_start(duk_context *ctx);
duk_ret_t duv_read_stop(duk_context *ctx);
duk_ret_t duv_write(duk_context *ctx);
duk_ret_t duv_write2(duk_context *ctx);
duk_ret_t duv_try_write(duk_context *ctx);
duk_ret_t duv_is_readable(duk_context *ctx);
duk_ret_t duv_is_writable(duk_context *ctx);
duk_ret_t duv_stream_set_blocking(duk_context *ctx);

#endif
