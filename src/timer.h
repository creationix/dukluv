#ifndef DUV_TIMER_H
#define DUV_TIMER_H

#include "duv.h"

duk_ret_t duv_new_timer(duk_context *ctx);
duk_ret_t duv_timer_start(duk_context *ctx);
duk_ret_t duv_timer_stop(duk_context *ctx);
duk_ret_t duv_timer_again(duk_context *ctx);
duk_ret_t duv_timer_set_repeat(duk_context *ctx);
duk_ret_t duv_timer_get_repeat(duk_context *ctx);

#endif
