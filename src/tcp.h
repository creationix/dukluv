#ifndef DUV_TCP_H
#define DUV_TCP_H

#include "duv.h"

duk_ret_t duv_new_tcp(duk_context *ctx);
duk_ret_t duv_tcp_open(duk_context *ctx);
duk_ret_t duv_tcp_nodelay(duk_context *ctx);
duk_ret_t duv_tcp_keepalive(duk_context *ctx);
duk_ret_t duv_tcp_simultaneous_accepts(duk_context *ctx);
duk_ret_t duv_tcp_bind(duk_context *ctx);
duk_ret_t duv_tcp_getsockname(duk_context *ctx);
duk_ret_t duv_tcp_getpeername(duk_context *ctx);
duk_ret_t duv_tcp_connect(duk_context *ctx);

#endif
