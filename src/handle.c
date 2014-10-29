#include "handle.h"

static void duv_close_cb(uv_handle_t* handle) {
  duk_context *ctx = handle->loop->data;
  duv_handle_t* data = handle->data;

  duv_emit_event(ctx, data, DUV_CLOSED, 0);
  handle->data = duv_cleanup_handle(ctx, data);
}

duk_ret_t duv_close(duk_context *ctx) {
  uv_handle_t* handle;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"handle", duv_is_handle},
    {"onclosed", dschema_is_continuation},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  uv_close(handle, duv_close_cb);
  duv_store_handler(ctx, handle->data, DUV_CLOSED, 1);
  return 0;
}
