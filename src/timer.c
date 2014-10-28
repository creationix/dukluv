#include "duv.h"

static duk_ret_t duv_new_timer(duk_context *ctx) {
  uv_timer_t* handle;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {NULL}
  });

  handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  duv_check(ctx, uv_timer_init(duv_loop(ctx), handle));
  handle->data = duv_setup_handle(ctx);
  return 1;
}

static void duv_timer_cb(uv_timer_t* handle) {
  duv_emit_event(handle->loop->data, handle->data, DUV_TIMEOUT, 0);
}

static duk_ret_t duv_timer_start(duk_context *ctx) {
  uv_timer_t* handle;
  uint64_t timeout;
  uint64_t repeat;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"timer", duv_is_timer},
    {"timeout", duk_is_number},
    {"repeat", duk_is_number},
    {"ontimeout", duk_is_callable},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  timeout = duk_get_number(ctx, 1);
  repeat = duk_get_number(ctx, 2);
  duv_check(ctx, uv_timer_start(handle, duv_timer_cb, timeout, repeat));
  duv_store_handler(ctx, handle->data, DUV_TIMEOUT, 3);
  return 0;
}

static duk_ret_t duv_timer_stop(duk_context *ctx) {
  uv_timer_t* handle;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"timer", duv_is_timer},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  duv_check(ctx, uv_timer_stop(handle));
  return 0;
}

static duk_ret_t duv_timer_again(duk_context *ctx) {
  uv_timer_t* handle;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"timer", duv_is_timer},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  duv_check(ctx, uv_timer_again(handle));
  return 0;
}

static duk_ret_t duv_timer_set_repeat(duk_context *ctx) {
  uv_timer_t* handle;
  uint64_t repeat;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"timer", duv_is_timer},
    {"repeat", duk_is_number},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  repeat = duk_get_number(ctx, 1);
  uv_timer_set_repeat(handle, repeat);
  return 0;
}

static duk_ret_t duv_timer_get_repeat(duk_context *ctx) {
  uv_timer_t* handle;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"timer", duv_is_timer},
    {NULL}
  });

  handle = duk_get_buffer(ctx, 0, NULL);
  duk_push_number(ctx, uv_timer_get_repeat(handle));
  return 1;
}
