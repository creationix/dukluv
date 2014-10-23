#include "duv.h"

static uv_pipe_t* duv_require_pipe(duk_context *ctx, int index) {
  // TODO: verify pointer is uv_handle_t* somehow.
  uv_pipe_t* handle = duk_require_buffer(ctx, index, NULL);
  if (handle->type != UV_NAMED_PIPE) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Expected uv_pipe_t");
  }
  return handle;
}

static duk_ret_t duv_new_pipe(duk_context *ctx) {
  uv_pipe_t* handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  int ipc = duk_require_boolean(ctx, 0);
  int ret = uv_pipe_init(duv_loop(ctx), handle, ipc);
  if (ret < 0) {
    duk_pop(ctx);
    duv_check(ctx, ret);
  }
  handle->data = duv_setup_handle(ctx);
  return 1;
}

static duk_ret_t duv_pipe_open(duk_context *ctx) {
  uv_pipe_t* handle = duv_require_pipe(ctx, 0);
  uv_file file = duk_require_int(ctx, 1);
  duv_check(ctx, uv_pipe_open(handle, file));
  return 0;
}

static duk_ret_t duv_pipe_bind(duk_context *ctx) {
  uv_pipe_t* handle = duv_require_pipe(ctx, 0);
  const char* name = duk_require_string(ctx, 1);
  duv_check(ctx, uv_pipe_bind(handle, name));
  return 0;
}

static duk_ret_t duv_pipe_connect(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_ERROR, "TODO: port pipe_connect");
  return 0;
  // uv_pipe_t* handle = duv_require_pipe(ctx, 0);
  // const char* name = duk_require_string(ctx, 1);
  // int ref = duv_check_continuation(ctx, 2);
  // uv_connect_t* req = lua_newuserdata(L, sizeof(*req));
  // req->data = duv_setup_req(L, ref);
  // uv_pipe_connect(req, handle, name, duv_connect_cb);
  // return 1;
}

static duk_ret_t duv_pipe_getsockname(duk_context *ctx) {
  uv_pipe_t* handle = duv_require_pipe(ctx, 0);
  size_t len = 2*PATH_MAX;
  char buf[2*PATH_MAX];
  duv_check(ctx, uv_pipe_getsockname(handle, buf, &len));
  duk_push_lstring(ctx, buf, len);
  return 1;
}

static duk_ret_t duv_pipe_pending_instances(duk_context *ctx) {
  uv_pipe_t* handle = duv_require_pipe(ctx, 0);
  int count = duk_require_int(ctx, 1);
  uv_pipe_pending_instances(handle, count);
  return 0;
}

static duk_ret_t duv_pipe_pending_count(duk_context *ctx) {
  uv_pipe_t* handle = duv_require_pipe(ctx, 0);
  duk_push_int(ctx, uv_pipe_pending_count(handle));
  return 1;
}

static duk_ret_t duv_pipe_pending_type(duk_context *ctx) {
  uv_pipe_t* handle = duv_require_pipe(ctx, 0);
  uv_handle_type type = uv_pipe_pending_type(handle);
  const char* type_name;
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
