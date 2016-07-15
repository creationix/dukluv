#include "uv_schema.h"

duk_bool_t duv_is_fd(duk_context* ctx, duk_idx_t index) {
  return duk_is_number(ctx, index);
}

duk_bool_t duv_is_handle(duk_context* ctx, duk_idx_t index) {
  // TODO: check for actual struct type somehow.
  return duk_is_fixed_buffer(ctx, index);
}

duk_bool_t duv_is_req(duk_context* ctx, duk_idx_t index) {
  // TODO: check for actual struct type somehow.
  return duk_is_fixed_buffer(ctx, index);
}

duk_bool_t duv_is_fs(duk_context* ctx, duk_idx_t index) {
  uv_fs_t* req;
  duk_size_t size;
  if (!duk_is_fixed_buffer(ctx, index)) return 0;
  req = duk_get_buffer(ctx, index, &size);
  // TODO: find safer check than size to verify struct type
  return size == sizeof(*req) &&
         req->type == UV_FS;
}

duk_bool_t duv_is_stream(duk_context* ctx, duk_idx_t index) {
  // TODO: check for actual struct type somehow.
  uv_stream_t* handle;
  if (!duk_is_fixed_buffer(ctx, index)) return 0;
  handle = duk_get_buffer(ctx, index, NULL);
  return handle->type == UV_TCP ||
         handle->type == UV_TTY ||
         handle->type == UV_NAMED_PIPE;
}

duk_bool_t duv_is_timer(duk_context* ctx, duk_idx_t index) {
  uv_timer_t* handle;
  duk_size_t size;
  if (!duk_is_fixed_buffer(ctx, index)) return 0;
  handle = duk_get_buffer(ctx, index, &size);
  // TODO: find safer check than size to verify struct type
  return size == sizeof(*handle) &&
         handle->type == UV_TIMER;
}

duk_bool_t duv_is_tcp(duk_context* ctx, duk_idx_t index) {
  uv_tcp_t* handle;
  duk_size_t size;
  if (!duk_is_fixed_buffer(ctx, index)) return 0;
  handle = duk_get_buffer(ctx, index, &size);
  // TODO: find safer check than size to verify struct type
  return size == sizeof(*handle) &&
         handle->type == UV_TCP;
}

duk_bool_t duv_is_pipe(duk_context* ctx, duk_idx_t index) {
  uv_pipe_t* handle;
  duk_size_t size;
  if (!duk_is_fixed_buffer(ctx, index)) return 0;
  handle = duk_get_buffer(ctx, index, &size);
  // TODO: find safer check than size to verify struct type
  return size == sizeof(*handle) &&
         handle->type == UV_NAMED_PIPE;
}

duk_bool_t duv_is_tty(duk_context* ctx, duk_idx_t index) {
  uv_tty_t* handle;
  duk_size_t size;
  if (!duk_is_fixed_buffer(ctx, index)) return 0;
  handle = duk_get_buffer(ctx, index, &size);
  // TODO: find safer check than size to verify struct type
  return size == sizeof(*handle) &&
         handle->type == UV_TTY;
}
