#include "duv.h"

static void duv_close_cb(uv_handle_t* handle) {
  duk_context *ctx = handle->loop->data;
  duv_handle_t* data = handle->data;
  duv_emit_event(ctx, handle, DUV_CLOSED, 0);
  duv_unref(ctx, data->ref);
  duv_unref(ctx, data->callbacks[0]);
  duv_unref(ctx, data->callbacks[1]);
  duk_free(ctx, data);
  duk_free(ctx, handle);
}

static duk_ret_t duv_close(duk_context *ctx) {
  // TODO: verify pointer type somehow.
  uv_handle_t *handle = duk_require_pointer(ctx, 0);
  duv_handle_t *data = handle->data;
  if (duk_is_function(ctx, 1)) {
    duk_dup(ctx, 1);
    data->callbacks[DUV_CLOSED] = duv_ref(ctx);
  }
  uv_close(handle, duv_close_cb);
  return 0;
}
