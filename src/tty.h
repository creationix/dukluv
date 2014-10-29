#ifndef DUV_TTY_H
#define DUV_TTY_H

#include "duv.h"

duk_ret_t duv_new_tty(duk_context *ctx);
duk_ret_t duv_tty_set_mode(duk_context *ctx);
duk_ret_t duv_tty_reset_mode(duk_context *ctx);
duk_ret_t duv_tty_get_winsize(duk_context *ctx);

#endif
