#include "utils.h"

uv_loop_t* duv_loop(duk_context *ctx) {
  duk_memory_functions funcs;
  duk_get_memory_functions(ctx, &funcs);
  return funcs.udata;
}

duv_handle_t* duv_setup_handle(duk_context *ctx) {
  duv_handle_t* data = duk_alloc(ctx, sizeof(*data));
  duk_push_this(ctx);
  data->context = duv_ref(ctx);
  duk_dup(ctx, -1);
  data->ref = duv_ref(ctx);
  data->callbacks[0] = 0;
  data->callbacks[1] = 0;
  return data;
}

duv_handle_t* duv_cleanup_handle(duk_context *ctx, duv_handle_t* data) {
  duv_unref(ctx, data->ref);
  duv_unref(ctx, data->context);
  duv_unref(ctx, data->callbacks[0]);
  duv_unref(ctx, data->callbacks[1]);
  duk_free(ctx, data);
  return NULL;
}

duv_req_t* duv_setup_req(duk_context *ctx, int callback_index) {
  duv_req_t* data = duk_alloc(ctx, sizeof(*data));
  duk_push_this(ctx);
  data->context = duv_ref(ctx);
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

duv_req_t* duv_cleanup_req(duk_context *ctx, duv_req_t *data) {
  duv_unref(ctx, data->req_ref);
  duv_unref(ctx, data->context);
  duv_unref(ctx, data->callback_ref);
  duk_free(ctx, data->data);
  duk_free(ctx, data);
  return NULL;
}


void duv_error(duk_context *ctx, int status) {
  duk_error(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
}

int duv_push_status(duk_context *ctx, int status) {
  if (status < 0) {
    return duk_push_error_object(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
  }
  duk_push_null(ctx);
  return 0;
}

void duv_check(duk_context *ctx, int status) {
  if (status < 0) duv_error(ctx, status);
}

void duv_store_handler(duk_context *ctx, duv_handle_t *data, int type, int index) {
  if (!duk_is_function(ctx, index)) return;
  duk_dup(ctx, index);
  data->callbacks[type] = duv_ref(ctx);
}

void duv_emit_event(duk_context *ctx, duv_handle_t* data, duv_callback_id type, int nargs) {
  int fn_ref = data->callbacks[type];
  if (fn_ref) {
    duv_push_ref(ctx, fn_ref);
    if (nargs) {
      duk_insert(ctx, -1 - nargs);
    }
    duv_push_ref(ctx, data->context);
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

void duv_fulfill_req(duk_context *ctx, uv_req_t* req, int nargs) {
  duv_req_t *data = req->data;
  if (data->callback_ref) {
    duv_push_ref(ctx, data->callback_ref);
    if (nargs) duk_insert(ctx, -1 - nargs);
    duv_push_ref(ctx, data->context);
    if (nargs) duk_insert(ctx, -1 - nargs);
    duk_call_method(ctx, nargs);
    duk_pop(ctx);
  }
  else if (nargs) {
    duk_pop_n(ctx, nargs);
  }
}

void duv_get_data(duk_context *ctx, int index, uv_buf_t* buf) {
  if (duk_is_string(ctx, index)) {
    buf->base = (char*) duk_get_lstring(ctx, index, &buf->len);
  }
  else {
    buf->base = duk_get_buffer(ctx, index, &buf->len);
  }
}

const char* duv_protocol_to_string(int family) {
#ifdef AF_UNIX
  if (family == AF_UNIX) return "UNIX";
#endif
#ifdef AF_INET
  if (family == AF_INET) return "INET";
#endif
#ifdef AF_INET6
  if (family == AF_INET6) return "INET6";
#endif
#ifdef AF_IPX
  if (family == AF_IPX) return "IPX";
#endif
#ifdef AF_NETLINK
  if (family == AF_NETLINK) return "NETLINK";
#endif
#ifdef AF_X25
  if (family == AF_X25) return "X25";
#endif
#ifdef AF_AX25
  if (family == AF_AX25) return "AX25";
#endif
#ifdef AF_ATMPVC
  if (family == AF_ATMPVC) return "ATMPVC";
#endif
#ifdef AF_APPLETALK
  if (family == AF_APPLETALK) return "APPLETALK";
#endif
#ifdef AF_PACKET
  if (family == AF_PACKET) return "PACKET";
#endif
  return NULL;
}
