#include "schema.h"

duk_bool_t duv_is_handle(duk_context* ctx, duk_idx_t index) {
  // TODO: check for actual struct type somehow.
  return duk_is_fixed_buffer(ctx, index);
}

duk_bool_t duv_is_req(duk_context* ctx, duk_idx_t index) {
  // TODO: check for actual struct type somehow.
  return duk_is_fixed_buffer(ctx, index);
}

duk_bool_t duv_is_fs(duk_context* ctx, duk_idx_t index) {
  // TODO: check for actual struct type somehow.
  uv_fs_t* req;
  if (!duk_is_fixed_buffer(ctx, index)) return 0;
  req = duk_get_buffer(ctx, index, NULL);
  return req->type == UV_FS;
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


duk_bool_t duv_is_data(duk_context* ctx, duk_idx_t index) {
  return duk_is_string(ctx, index) || duk_is_buffer(ctx, index);
}

duk_bool_t duv_is_continuation(duk_context* ctx, duk_idx_t index) {
  return !duk_is_valid_index(ctx, index) ||
          duk_is_callable(ctx, index) ||
          duk_is_undefined(ctx, index);
}

void duv_check_args(duk_context *ctx, const duv_schema_entry schema[]) {
  int i;
  int top = duk_get_top(ctx);
  for (i = 0; schema[i].name; ++i) {
    // printf("Checking arg %d-%s\n", i, schema[i].name);
    if (schema[i].checker(ctx, i)) continue;
    duk_dump_context_stderr(ctx);
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid argument type for %s", schema[i].name);
  }
  if (top > i) {
    duk_dump_context_stderr(ctx);
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Too many arguments. Expected at %d, but got %d", i, top);
  }
}
