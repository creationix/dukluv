#include "duktape.h"
#include "uv.h"

static uv_loop_t* duv_loop(duk_context *ctx) {
  duk_memory_functions funcs;
  duk_get_memory_functions(ctx, &funcs);
  return funcs.udata;
}

static void* duv_setup_handle(duk_context *ctx) {
  // TODO: keep the handle safe
  return NULL;
}

static int duv_error(duk_context *ctx, int status) {
  fprintf(stderr, "TODO: throw libuv exception\n");
  exit(status);
}

static duk_ret_t duv_new_timer(duk_context *ctx) {

  uv_timer_t* handle = duk_push_fixed_buffer(ctx, sizeof(*handle));
  int ret = uv_timer_init(duv_loop(ctx), handle);
  if (ret < 0) {
    duk_pop(ctx);
    return duv_error(ctx, ret);
  }
  handle->data = duv_setup_handle(ctx);
  return 1;
}

static void duv_timer_cb(uv_timer_t* handle) {
  printf("TIMEOUT! %p\n", handlegit);
}

static duk_ret_t duv_timer_start(duk_context *ctx) {
  uv_timer_t* handle = duk_require_buffer(ctx, 0, NULL);
  int delay = duk_require_int(ctx, 1);
  int repeat = duk_require_int(ctx, 2);
  int ret = uv_timer_start(handle, duv_timer_cb, delay, repeat);
  if (ret < 0) {
    duk_pop(ctx);
    return duv_error(ctx, ret);
  }
  handle->data = duv_setup_handle(ctx);
  return 1;
}

static duk_ret_t duv_run(duk_context *ctx) {
  int ret = uv_run(duv_loop(ctx), UV_RUN_DEFAULT);
  if (ret < 0) {
    return duv_error(ctx, ret);
  }
  return 0;
}

int main(int argc, const char *argv[]) {
  duk_context *ctx = NULL;

  if (argc < 2) {
    fprintf(stderr, "Usage: dukluv script.js\n");
    exit(1);
  }

  ctx = duk_create_heap(NULL, NULL, NULL, uv_default_loop(), NULL);
  if (!ctx) {
    fprintf(stderr, "Problem initiailizing duktape heap\n");
    exit(1);
  }

  duk_push_global_object(ctx);

  duk_push_object(ctx);

  duk_push_c_function(ctx, duv_new_timer, 0);
  duk_put_prop_string(ctx, -2, "new_timer");

  duk_push_c_function(ctx, duv_run, 0);
  duk_put_prop_string(ctx, -2, "run");

  duk_put_prop_string(ctx, -2, "uv");

  char* b = duk_push_fixed_buffer(ctx, 10);
  int i;
  for (i = 0; i < 10; i++) {
    b[i] = 10 - 1;
  }
  duk_put_prop_string(ctx, -2, "b");

  duk_push_global_object(ctx);
  duk_put_prop_string(ctx, -2, "global");

  duk_eval_file_noresult(ctx, argv[1]);

  duk_destroy_heap(ctx);

  return 0;
}
