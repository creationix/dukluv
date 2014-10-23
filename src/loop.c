#include "duv.h"

static duk_ret_t duv_run(duk_context *ctx) {
  int ret = uv_run(duv_loop(ctx), UV_RUN_DEFAULT);
  if (ret < 0) {
    duv_error(ctx, ret);
  }
  return 0;
}

static void duv_walk_cb(uv_handle_t *handle, duk_context *ctx) {
  duk_dup(ctx, 0);
  duv_handle_t* data = handle->data;
  duv_push_ref(ctx, data->ref);
  duk_call(ctx, 1);
  duk_pop(ctx);
}

static duk_ret_t duv_walk(duk_context *ctx) {
  if (!duk_is_function(ctx, 0)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "callback required");
  }
  uv_walk(duv_loop(ctx), duv_walk_cb, ctx);
  return 0;
}
