#include "loop.h"

duk_ret_t duv_run(duk_context *ctx) {
  int ret = uv_run(duv_loop(ctx), UV_RUN_DEFAULT);
  if (ret < 0) {
    duv_error(ctx, ret);
  }
  return 0;
}

static void duv_walk_cb(uv_handle_t *handle, void* c) {
  duk_context *ctx = c;
  duv_handle_t* data = handle->data;
  duk_dup(ctx, 0);
  duv_push_ref(ctx, data->context);
  duv_push_ref(ctx, data->ref);
  duk_call_method(ctx, 1);
}

duk_ret_t duv_walk(duk_context *ctx) {

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", duk_is_callable},
    {NULL}
  });

  uv_walk(duv_loop(ctx), duv_walk_cb, ctx);
  return 0;
}
