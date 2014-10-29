#include "tty.h"

static uv_tty_t* duv_require_tty(duk_context *ctx, int index) {
  // TODO: verify pointer is uv_handle_t* somehow.
  uv_tty_t* handle = duk_require_buffer(ctx, index, NULL);
  if (handle->type != UV_TTY) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Expected uv_tty_t");
  }
  return handle;
}

duk_ret_t duv_new_tty(duk_context *ctx) {
  uv_tty_t* handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  uv_file fd = duk_require_int(ctx, 0);
  duk_bool_t readable = duk_require_boolean(ctx, 1);
  int ret = uv_tty_init(duv_loop(ctx), handle, fd, readable);
  if (ret < 0) {
    duk_pop(ctx);
    duv_check(ctx, ret);
  }
  handle->data = duv_setup_handle(ctx);
  return 1;
}

duk_ret_t duv_tty_set_mode(duk_context *ctx) {
  uv_tty_t* handle = duv_require_tty(ctx, 0);
  int mode = duk_require_int(ctx, 1);
  duv_check(ctx, uv_tty_set_mode(handle, mode));
  return 0;
}

duk_ret_t duv_tty_reset_mode(duk_context *ctx) {
  duv_check(ctx, uv_tty_reset_mode());
  return 0;
}

duk_ret_t duv_tty_get_winsize(duk_context *ctx) {
  uv_tty_t* handle = duv_require_tty(ctx, 0);
  int width, height;
  duv_check(ctx, uv_tty_get_winsize(handle, &width, &height));
  duk_push_object(ctx);
  duk_push_int(ctx, width);
  duk_put_prop_string(ctx, -2, "width");
  duk_push_int(ctx, height);
  duk_put_prop_string(ctx, -2, "height");
  return 1;
}
