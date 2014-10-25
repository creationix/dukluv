#include "duv.h"
#include <assert.h>

static uv_stream_t* duv_require_stream(duk_context *ctx, int index) {
  uv_stream_t* handle = duk_require_buffer(ctx, index, NULL);
  if (handle->type != UV_TCP &&
      handle->type != UV_TTY &&
      handle->type != UV_NAMED_PIPE) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Expected uv_stream_t subclass");
  }
  return handle;
}

static void duv_shutdown_cb(uv_shutdown_t* req, int status) {
  duk_context *ctx = req->handle->loop->data;
  duv_push_status(ctx, status);
  duv_fulfill_req(ctx, (uv_req_t*)req, 1);
}

static duk_ret_t duv_shutdown(duk_context *ctx) {
  uv_stream_t* handle = duv_require_stream(ctx, 0);
  uv_shutdown_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  duv_check(ctx, uv_shutdown(req, handle, duv_shutdown_cb));
  req->data = duv_setup_req(ctx, 1);
  return 1;
}

static void duv_connection_cb(uv_stream_t* handle, int status) {
  duk_context *ctx = handle->loop->data;
  duv_push_status(ctx, status);
  duv_emit_event(ctx, handle->data, DUV_CONNECTION, 1);
}

static duk_ret_t duv_listen(duk_context *ctx) {
  uv_stream_t* handle = duv_require_stream(ctx, 0);
  int backlog = duk_require_int(ctx, 1);
  duv_require_callback(ctx, handle->data, DUV_CONNECTION, 2);
  duv_check(ctx, uv_listen(handle, backlog, duv_connection_cb));
  return 0;
}

static duk_ret_t duv_accept(duk_context *ctx) {
  uv_stream_t* server = duv_require_stream(ctx, 0);
  uv_stream_t* client = duv_require_stream(ctx, 1);
  duv_check(ctx, uv_accept(server, client));
  return 0;
}

static void duv_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  buf->base = malloc(suggested_size);
  assert(buf->base);
  buf->len = suggested_size;
}

static void duv_read_cb(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
  duk_context *ctx = handle->loop->data;

  duv_handle_t* data = handle->data;
  duv_push_ref(ctx, data->ref);

  if (nread >= 0) {
    duk_push_null(ctx);
    duk_push_lstring(ctx, buf->base, nread);
  }

  free(buf->base);
  if (nread == 0) return;

  if (nread == UV_EOF) {
    duk_push_null(ctx); // no error
    duk_push_undefined(ctx); // undefined value to signify EOF
  }
  else if (nread < 0) {
    duv_push_status(ctx, nread);
  }

  duv_emit_event(ctx, handle->data, DUV_READ, 2);
}

static duk_ret_t duv_read_start(duk_context *ctx) {
  uv_stream_t* handle = duv_require_stream(ctx, 0);
  duv_require_callback(ctx, handle->data, DUV_READ, 1);
  duv_check(ctx, uv_read_start(handle, duv_alloc_cb, duv_read_cb));
  return 0;
}

static duk_ret_t duv_read_stop(duk_context *ctx) {
  uv_stream_t* handle = duv_require_stream(ctx, 0);
  duv_check(ctx, uv_read_stop(handle));
  return 0;
}

static void duv_write_cb(uv_write_t* req, int status) {
  duk_context *ctx = req->handle->loop->data;
  duv_push_status(ctx, status);
  duv_fulfill_req(ctx, req->data, 1);
  req->data = duv_cleanup_req(ctx, req->data);
}

static duk_ret_t duv_write(duk_context *ctx) {
  uv_stream_t* handle = duv_require_stream(ctx, 0);
  uv_buf_t buf;
  int ret;
  buf.base = (char*) duk_require_lstring(ctx, 1, &buf.len);
  uv_write_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  ret = uv_write(req, handle, &buf, 1, duv_write_cb);
  if (ret < 0) {
    duk_pop(ctx);
    duv_error(ctx, ret);
  }
  req->data = duv_setup_req(ctx, 2);
  return 1;
}

// static duk_ret_t duv_write2(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   uv_write_t* req;
//   uv_buf_t buf;
//   int ret, ref;
//   uv_stream_t* send_handle;
//   buf.base = (char*) luaL_checklstring(L, 2, &buf.len);
//   send_handle = duv_require_stream(L, 3);
//   ref = duv_check_continuation(L, 4);
//   req = lua_newuserdata(L, sizeof(*req));
//   req->data = duv_setup_req(L, ref);
//   ret = uv_write2(req, handle, &buf, 1, send_handle, duv_write_cb);
//   if (ret < 0) {
//     lua_pop(L, 1);
//     return duv_error(L, ret);
//   }
//   return 1;
// }

// static duk_ret_t duv_try_write(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   uv_buf_t buf;
//   int ret;
//   buf.base = (char*) luaL_checklstring(L, 2, &buf.len);
//   ret = uv_try_write(handle, &buf, 1);
//   if (ret < 0) return duv_error(L, ret);
//   lua_pushinteger(L, ret);
//   return 1;
// }

// static duk_ret_t duv_is_readable(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   lua_pushboolean(L, uv_is_readable(handle));
//   return 1;
// }

// static duk_ret_t duv_is_writable(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   lua_pushboolean(L, uv_is_writable(handle));
//   return 1;
// }

// static duk_ret_t duv_stream_set_blocking(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   int blocking, ret;
//   luaL_checktype(L, 2, LUA_TBOOLEAN);
//   blocking = lua_toboolean(L, 2);
//   ret = uv_stream_set_blocking(handle, blocking);
//   if (ret < 0) return duv_error(L, ret);
//   lua_pushinteger(L, ret);
//   return 1;
// }

