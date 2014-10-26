#include "utils.h"

static uv_loop_t* duv_loop(duk_context *ctx) {
  duk_memory_functions funcs;
  duk_get_memory_functions(ctx, &funcs);
  return funcs.udata;
}

static duv_handle_t* duv_setup_handle(duk_context *ctx) {
  duv_handle_t* data = duk_alloc(ctx, sizeof(*data));
  duk_dup(ctx, -1);
  data->ref = duv_ref(ctx);
  data->callbacks[0] = 0;
  data->callbacks[1] = 0;
  return data;
}

static duv_handle_t* duv_cleanup_handle(duk_context *ctx, duv_handle_t* data) {
  duv_unref(ctx, data->ref);
  duv_unref(ctx, data->callbacks[0]);
  duv_unref(ctx, data->callbacks[1]);
  duk_free(ctx, data);
  return NULL;
}

static duv_req_t* duv_setup_req(duk_context *ctx, int callback_index) {
  duv_req_t* data = duk_alloc(ctx, sizeof(*data));
  duk_dup(ctx, -1);
  data->req_ref = duv_ref(ctx);
  if (duk_is_callable(ctx, callback_index)) {
    duk_dup(ctx, callback_index);
    data->callback_ref = duv_ref(ctx);
  }
  else {
    data->callback_ref = 0;
  }
  data->data = NULL;
  return data;
}

static duv_req_t* duv_cleanup_req(duk_context *ctx, duv_req_t *data) {
  duv_unref(ctx, data->req_ref);
  duv_unref(ctx, data->callback_ref);
  duk_free(ctx, data->data);
  duk_free(ctx, data);
  return NULL;
}


static void duv_error(duk_context *ctx, int status) {
  duk_error(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
}

static int duv_push_status(duk_context *ctx, int status) {
  if (status < 0) {
    return duk_push_error_object(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
  }
  duk_push_null(ctx);
  return 0;
}

static void duv_check(duk_context *ctx, int status) {
  if (status < 0) duv_error(ctx, status);
}

static void duv_store_handler(duk_context *ctx, duv_handle_t *data, int type, int index) {
  if (!duk_is_function(ctx, index)) return;
  duk_dup(ctx, index);
  data->callbacks[type] = duv_ref(ctx);
}

static void duv_emit_event(duk_context *ctx, duv_handle_t* data, duv_callback_id type, int nargs) {
  int fn_ref = data->callbacks[type];
  if (fn_ref) {
    duv_push_ref(ctx, fn_ref);
    if (nargs) {
      duk_insert(ctx, -1 - nargs);
    }
    duv_push_ref(ctx, data->ref);
    if (nargs) {
      duk_insert(ctx, -1 - nargs);
    }
    duk_call_method(ctx, nargs);
    duk_pop(ctx);
  }
  else if (nargs) {
    duk_pop_n(ctx, nargs);
  }
}

static void duv_fulfill_req(duk_context *ctx, uv_req_t* req, int nargs) {
  duv_req_t *data = req->data;
  if (data->callback_ref) {
    duv_push_ref(ctx, data->callback_ref);
    if (nargs) duk_insert(ctx, -1 - nargs);
    duk_call(ctx, nargs);
    duk_pop(ctx);
  }
  else if (nargs) {
    duk_pop_n(ctx, nargs);
  }
}
