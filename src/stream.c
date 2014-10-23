#include "duv.h"

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
  uv_shutdown_t* req;
  int ret;
  if (!(duk_is_undefined(ctx, 1) || duk_is_function(ctx, 1))) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Optional continuation callback must be function");
  }
  req = duk_alloc(ctx, sizeof(*req));
  ret = uv_shutdown(req, handle, duv_shutdown_cb);
  if (ret < 0) {
    duk_free(ctx, req);
    duv_check(ctx, ret);
  }
  duv_req_t* data = duk_alloc(ctx, sizeof(*data));
  duk_dup(ctx, 0);
  data->req_ref = duv_ref(ctx);
  duk_dup(ctx, 1);
  data->callback_ref = duv_ref(ctx);
  req->data = data;
  duk_push_pointer(ctx, req);
  return 1;
}

// static void duv_connection_cb(uv_stream_t* handle, int status) {
//   duk_context *ctx = duv_state(handle->loop);
//   duv_find_handle(L, handle->data);
//   duv_status(L, status);
//   duv_call_callback(L, handle->data, duv_CONNECTION, 2);
// }

// static duk_ret_t duv_listen(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   int backlog = luaL_checkinteger(L, 2);
//   int ret;
//   duv_check_callback(L, handle->data, duv_CONNECTION, 3);
//   ret = uv_listen(handle, backlog, duv_connection_cb);
//   if (ret < 0) return duv_error(L, ret);
//   lua_pushinteger(L, ret);
//   return 1;
// }

// static duk_ret_t duv_accept(duk_context *ctx) {
//   uv_stream_t* server = duv_require_stream(L, 1);
//   uv_stream_t* client = duv_require_stream(L, 2);
//   int ret = uv_accept(server, client);
//   if (ret < 0) return duv_error(L, ret);
//   lua_pushinteger(L, ret);
//   return 1;
// }

// static void duv_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
//   buf->base = malloc(suggested_size);
//   assert(buf->base);
//   buf->len = suggested_size;
// }

// static void duv_read_cb(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
//   duk_context *ctx = duv_state(handle->loop);
//   duv_find_handle(L, handle->data);

//   if (nread >= 0) {
//     lua_pushnil(L);
//     lua_pushlstring(L, buf->base, nread);
//   }

//   free(buf->base);
//   if (nread == 0) return;

//   if (nread == UV_EOF) {
//     lua_pushnil(L); // no error
//     lua_pushnil(L); // nil value to signify EOF
//   }
//   else if (nread < 0) {
//     duv_status(L, nread);
//   }

//   duv_call_callback(L, handle->data, duv_READ, 3);
// }

// static duk_ret_t duv_read_start(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   int ret;
//   duv_check_callback(L, handle->data, duv_READ, 2);
//   ret = uv_read_start(handle, duv_alloc_cb, duv_read_cb);
//   if (ret < 0) return duv_error(L, ret);
//   lua_pushinteger(L, ret);
//   return 1;
// }

// static duk_ret_t duv_read_stop(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   int ret = uv_read_stop(handle);
//   if (ret < 0) return duv_error(L, ret);
//   lua_pushinteger(L, ret);
//   return 1;
// }

// static void duv_write_cb(uv_write_t* req, int status) {
//   duk_context *ctx = duv_state(req->handle->loop);
//   duv_find_handle(L, req->handle->data);
//   duv_status(L, status);
//   duv_fulfill_req(L, req->data, 2);
//   duv_cleanup_req(L, req->data);
//   req->data = NULL;
// }

// static duk_ret_t duv_write(duk_context *ctx) {
//   uv_stream_t* handle = duv_require_stream(L, 1);
//   uv_write_t* req;
//   uv_buf_t buf;
//   int ret, ref;
//   buf.base = (char*) luaL_checklstring(L, 2, &buf.len);
//   ref = duv_check_continuation(L, 3);
//   req = lua_newuserdata(L, sizeof(*req));
//   req->data = duv_setup_req(L, ref);
//   ret = uv_write(req, handle, &buf, 1, duv_write_cb);
//   if (ret < 0) {
//     lua_pop(L, 1);
//     return duv_error(L, ret);
//   }
//   return 1;
// }

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

