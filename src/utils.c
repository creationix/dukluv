#include "utils.h"

static uv_loop_t* duv_loop(duk_context *ctx) {
  duk_memory_functions funcs;
  duk_get_memory_functions(ctx, &funcs);
  return funcs.udata;
}

static void duv_setup_handle(duk_context *ctx, uv_handle_t *handle) {
  duv_handle_t* data = duk_alloc(ctx, sizeof(*data));
  handle->data = data;
  duk_push_pointer(ctx, handle);
  duk_dup(ctx, -1);
  data->ref = duv_ref(ctx);
  data->callbacks[0] = 0;
  data->callbacks[1] = 0;
}

static int duv_error(duk_context *ctx, int status) {
  duk_error(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
  return 0;
}

static void duv_check(duk_context *ctx, int status) {
  if (status < 0) duv_error(ctx, status);
}

static void duv_emit_event(duk_context *ctx, uv_handle_t* handle, duv_callback_id type, int nargs) {
  duv_handle_t* data = handle->data;
  int ref = data->callbacks[type];
  if (ref) {
    duv_push_ref(ctx, ref);
    if (nargs) duk_insert(ctx, -1 - nargs);
    duk_push_pointer(ctx, handle);
    if (nargs) duk_insert(ctx, -1 - nargs);
    duk_call_method(ctx, nargs);
    duk_pop(ctx);
  }
}
