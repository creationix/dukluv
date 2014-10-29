#include "tcp.h"

duk_ret_t duv_new_tcp(duk_context *ctx) {
  uv_tcp_t* handle;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {NULL}
  });

  handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  duv_check(ctx, uv_tcp_init(duv_loop(ctx), handle));
  handle->data = duv_setup_handle(ctx);
  return 1;
}

duk_ret_t duv_tcp_open(duk_context *ctx) {
  uv_tcp_t* handle;
  uv_os_sock_t sock;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {"socket", duk_is_number},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  sock = duk_get_number(ctx, 1);
  duv_check(ctx, uv_tcp_open(handle, sock));
  return 0;
}

duk_ret_t duv_tcp_nodelay(duk_context *ctx) {
  uv_tcp_t* handle;
  int enable;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {"isenabled", duk_is_boolean},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  enable = duk_get_boolean(ctx, 1);
  duv_check(ctx, uv_tcp_nodelay(handle, enable));
  return 0;
}

duk_ret_t duv_tcp_keepalive(duk_context *ctx) {
  uv_tcp_t* handle;
  int enable, delay;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {"isenabled", duk_is_boolean},
    {"delay", duk_is_number},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  enable = duk_get_boolean(ctx, 1);
  delay = duk_get_number(ctx, 2);
  duv_check(ctx, uv_tcp_keepalive(handle, enable, delay));
  return 0;
}

duk_ret_t duv_tcp_simultaneous_accepts(duk_context *ctx) {
  uv_tcp_t* handle;
  int enable;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {"isenabled", duk_is_boolean},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  enable = duk_get_boolean(ctx, 1);
  duv_check(ctx, uv_tcp_simultaneous_accepts(handle, enable));
  return 0;
}

duk_ret_t duv_tcp_bind(duk_context *ctx) {
  uv_tcp_t* handle;
  const char* host;
  int port, flags;
  struct sockaddr_storage addr;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {"host", duk_is_string},
    {"port", duk_is_number},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  host = duk_get_string(ctx, 1);
  port = duk_get_number(ctx, 2);
  flags = 0;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
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
  duk_push_number(ctx, port);
  duk_put_prop_string(ctx, -2, "port");
  duk_push_string(ctx, ip);
  duk_put_prop_string(ctx, -2, "ip");
}

duk_ret_t duv_tcp_getsockname(duk_context *ctx) {
  uv_tcp_t* handle;
  int addrlen;
  struct sockaddr_storage address;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  addrlen = sizeof(address);
  duv_check(ctx, uv_tcp_getsockname(handle, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

duk_ret_t duv_tcp_getpeername(duk_context *ctx) {
  uv_tcp_t* handle;
  int addrlen;
  struct sockaddr_storage address;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  addrlen = sizeof(address);
  duv_check(ctx, uv_tcp_getpeername(handle, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

static void duv_connect_cb(uv_connect_t* req, int status) {
  duk_context *ctx = req->handle->loop->data;
  duv_push_status(ctx, status);
  duv_fulfill_req(ctx, (uv_req_t*)req, 1);
  req->data = duv_cleanup_req(ctx, req->data);
}

duk_ret_t duv_tcp_connect(duk_context *ctx) {
  uv_tcp_t* handle;
  const char* host;
  int port, flags;
  struct sockaddr_storage addr;
  uv_connect_t* req;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"tcp", duv_is_tcp},
    {"host", duk_is_string},
    {"port", duk_is_number},
    {"next", duv_is_continuation},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  host = duk_get_string(ctx, 1);
  port = duk_get_number(ctx, 2);
  flags = 0;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
  }

  handle = duk_get_buffer(ctx, 0, NULL);
  req = duk_push_fixed_buffer(ctx, sizeof(*req));
  duv_check(ctx,
    uv_tcp_connect(req, handle, (struct sockaddr*)&addr, duv_connect_cb));
  req->data = duv_setup_req(ctx, 3);
  return 1;
}
