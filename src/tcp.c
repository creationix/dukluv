#include "duv.h"

static uv_tcp_t* duv_require_tcp(duk_context *ctx, int index) {
  // TODO: verify pointer is uv_handle_t* somehow.
  uv_tcp_t* handle = duk_require_buffer(ctx, index, NULL);
  if (handle->type != UV_TCP) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Expected uv_tcp_t");
  }
  return handle;
}

static duk_ret_t duv_new_tcp(duk_context *ctx) {
  uv_tcp_t* handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  int ret = uv_tcp_init(duv_loop(ctx), handle);
  if (ret < 0) {
    duk_pop(ctx);
    duv_check(ctx, ret);
  }
  handle->data = duv_setup_handle(ctx);
  return 1;
}

static duk_ret_t duv_tcp_open(duk_context *ctx) {
  uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  uv_os_sock_t sock = duk_require_int(ctx, 1);
  duv_check(ctx, uv_tcp_open(handle, sock));
  return 0;
}

static duk_ret_t duv_tcp_nodelay(duk_context *ctx) {
  uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  int enable = duk_require_boolean(ctx, 1);
  duv_check(ctx, uv_tcp_nodelay(handle, enable));
  return 0;
}

static duk_ret_t duv_tcp_keepalive(duk_context *ctx) {
  uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  int enable = duk_require_boolean(ctx, 1);
  unsigned int delay = duk_require_uint(ctx, 2);
  duv_check(ctx, uv_tcp_keepalive(handle, enable, delay));
  return 0;
}

static duk_ret_t duv_tcp_simultaneous_accepts(duk_context *ctx) {
  uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  int enable = duk_require_boolean(ctx, 1);
  duv_check(ctx, uv_tcp_simultaneous_accepts(handle, enable));
  return 0;
}

static duk_ret_t duv_tcp_bind(duk_context *ctx) {
  uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  const char* host = duk_require_string(ctx, 1);
  unsigned int port = duk_require_uint(ctx, 2);
  unsigned int flags = 0;
  struct sockaddr_storage addr;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
  }
  if (duk_is_object(ctx, 3)) {
    duk_get_prop_string(ctx, 3, "ipv6only");
    if (duk_to_boolean(ctx, -1)) flags |= UV_TCP_IPV6ONLY;
    duk_pop(ctx);
  }
  duv_check(ctx, uv_tcp_bind(handle, (struct sockaddr*)&addr, flags));
  return 0;
}

static void duv_push_sockaddr(duk_context *ctx, struct sockaddr_storage* address, int addrlen) {
  char ip[INET6_ADDRSTRLEN];
  int port = 0;
  if (address->ss_family == AF_INET) {
    struct sockaddr_in* addrin = (struct sockaddr_in*)address;
    uv_inet_ntop(AF_INET, &(addrin->sin_addr), ip, addrlen);
    port = ntohs(addrin->sin_port);
  } else if (address->ss_family == AF_INET6) {
    struct sockaddr_in6* addrin6 = (struct sockaddr_in6*)address;
    uv_inet_ntop(AF_INET6, &(addrin6->sin6_addr), ip, addrlen);
    port = ntohs(addrin6->sin6_port);
  }

  duk_push_object(ctx);
  duk_push_string(ctx, duv_protocol_to_string(address->ss_family));
  duk_put_prop_string(ctx, -2, "family");
  duk_push_uint(ctx, port);
  duk_put_prop_string(ctx, -2, "port");
  duk_push_string(ctx, ip);
  duk_put_prop_string(ctx, -2, "ip");
}

static duk_ret_t duv_tcp_getsockname(duk_context *ctx) {
  uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  duv_check(ctx, uv_tcp_getsockname(handle, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

static duk_ret_t duv_tcp_getpeername(duk_context *ctx) {
  uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  duv_check(ctx, uv_tcp_getpeername(handle, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

// static void duv_connect_cb(uv_connect_t* req, int status) {
//   duk_context *ctx = duv_state(req->handle->loop);
//   duv_find_handle(L, req->handle->data);
//   duv_status(L, status);
//   duv_fulfill_req(L, req->data, 2);
//   duv_cleanup_req(L, req->data);
//   req->data = NULL;
// }

static duk_ret_t duv_tcp_connect(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_ERROR, "TODO: port tcp_connect");
  return 0;
  // uv_tcp_t* handle = duv_require_tcp(ctx, 0);
  // const char* host = luaL_checkstring(L, 2);
  // int port = duk_require_int(L, 3);
  // struct sockaddr_storage addr;
  // uv_connect_t* req;
  // int ret, ref;
  // if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
  //     uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
  //   return luaL_argerror(L, 3, "Invalid IP address or port");
  // }
  // ref = duv_check_continuation(L, 4);

  // req = lua_newuserdata(L, sizeof(*req));
  // req->data = duv_setup_req(L, ref);
  // ret = uv_tcp_connect(req, handle, (struct sockaddr*)&addr, duv_connect_cb);
  // if (ret < 0) {
  //   lua_pop(L, 1);
  //   return duv_error(L, ret);
  // }
  // return 1;
}
