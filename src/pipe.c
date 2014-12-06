#include "duv.h"

duk_ret_t duv_new_pipe(duk_context *ctx) {
  uv_pipe_t* handle;
  int ipc;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"ipc", duk_is_boolean},
    {NULL}
  });

  handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  ipc = duk_get_boolean(ctx, 0);
  duv_check(ctx, uv_pipe_init(duv_loop(ctx), handle, ipc));
  handle->data = duv_setup_handle(ctx);
  return 1;
}

duk_ret_t duv_pipe_open(duk_context *ctx) {
  uv_pipe_t* handle;
  uv_file file;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"pipe", duv_is_pipe},
    {"fd", duv_is_fd},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  file = duk_get_int(ctx, 1);
  duv_check(ctx, uv_pipe_open(handle, file));
  return 0;
}

duk_ret_t duv_pipe_bind(duk_context *ctx) {
  uv_pipe_t* handle;
  const char* name;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"pipe", duv_is_pipe},
    {"name", duk_is_string},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  name = duk_get_string(ctx, 1);
  duv_check(ctx, uv_pipe_bind(handle, name));
  return 0;
}

duk_ret_t duv_pipe_connect(duk_context *ctx) {
  uv_connect_t* req;
  uv_pipe_t* handle;
  const char* name;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"pipe", duv_is_pipe},
    {"name", duk_is_string},
    {"next", dschema_is_continuation},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  name = duk_get_string(ctx, 1);

  req = duk_push_fixed_buffer(ctx, sizeof(*req));
  uv_pipe_connect(req, handle, name, duv_connect_cb);
  req->data = duv_setup_req(ctx, 2);
  return 1;
}

duk_ret_t duv_pipe_getsockname(duk_context *ctx) {
  uv_pipe_t* handle;
  size_t len = 2*PATH_MAX;
  char buf[2*PATH_MAX];

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"pipe", duv_is_pipe},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  duv_check(ctx, uv_pipe_getsockname(handle, buf, &len));
  duk_push_lstring(ctx, buf, len);
  return 1;
}

duk_ret_t duv_pipe_pending_instances(duk_context *ctx) {
  uv_pipe_t* handle;
  int count;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"pipe", duv_is_pipe},
    {"count", duk_is_number},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  count = duk_get_int(ctx, 1);
  uv_pipe_pending_instances(handle, count);
  return 0;
}

duk_ret_t duv_pipe_pending_count(duk_context *ctx) {
  uv_pipe_t* handle;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"pipe", duv_is_pipe},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  duk_push_int(ctx, uv_pipe_pending_count(handle));
  return 1;
}

duk_ret_t duv_pipe_pending_type(duk_context *ctx) {
  uv_pipe_t* handle;
  uv_handle_type type;
  const char* type_name;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"pipe", duv_is_pipe},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  type = uv_pipe_pending_type(handle);
  switch (type) {
#define XX(uc, lc) \
    case UV_##uc: type_name = #uc; break;
  UV_HANDLE_TYPE_MAP(XX)
#undef XX
    default: return 0;
  }
  duk_push_string(ctx, type_name);
  return 1;
}
