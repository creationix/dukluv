#include "duv.h"

static uv_loop_t loop;

// Sync readfile using libuv APIs as an API function.
static duk_ret_t duv_loadfile(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  uv_fs_t req;
  int fd = 0;
  uint64_t size;
  char* chunk;
  uv_buf_t buf;

  if (uv_fs_open(&loop, &req, path, O_RDONLY, 0644, NULL) < 0) goto fail;
  fd = req.result;
  if (uv_fs_fstat(&loop, &req, fd, NULL) < 0) goto fail;
  size = req.statbuf.st_size;
  chunk = duk_alloc(ctx, size);
  buf = uv_buf_init(chunk, size);
  if (uv_fs_read(&loop, &req, fd, &buf, 1, 0, NULL) < 0) goto fail;
  duk_push_lstring(ctx, chunk, size);
  duk_free(ctx, chunk);
  uv_fs_close(&loop, &req, fd, NULL);
  uv_fs_req_cleanup(&req);

  return 1;

  fail:
  if (fd) uv_fs_close(&loop, &req, fd, NULL);
  uv_fs_req_cleanup(&req);
  duk_error(ctx, DUK_ERR_ERROR, "%s: %s: %s", uv_err_name(req.result), uv_strerror(req.result), path);
}

static duk_ret_t duv_main(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);

  duk_push_global_object(ctx);
  duk_dup(ctx, -1);
  duk_put_prop_string(ctx, -2, "global");
  dukopen_duv(ctx);
  duk_put_prop_string(ctx, -2, "uv");

  // Setup the module loader
  duk_get_prop_string(ctx, -1, "Duktape");
  duk_push_c_function(ctx, duv_loadfile, 1);
  duk_put_prop_string(ctx, -2, "modSearch");
  duk_pop(ctx);

  duk_eval_file_noresult(ctx, path);
  uv_run(&loop, UV_RUN_DEFAULT);

  return 0;

}

int main(int argc, char *argv[]) {
  duk_context *ctx = NULL;
  uv_loop_init(&loop);

  uv_setup_args(argc, argv);

  if (argc < 2) {
    fprintf(stderr, "Usage: dukluv script.js\n");
    exit(1);
  }

  // Tie loop and context together
  ctx = duk_create_heap(NULL, NULL, NULL, &loop, NULL);
  if (!ctx) {
    fprintf(stderr, "Problem initiailizing duktape heap\n");
    return -1;
  }
  loop.data = ctx;

  duk_push_c_function(ctx, duv_main, 1);
  duk_push_string(ctx, argv[1]);
  if (duk_pcall(ctx, 1)) {
    fprintf(stderr, "\nUncaught Exception:\n");
    if (duk_is_object(ctx, -1)) {
      duk_get_prop_string(ctx, -1, "stack");
      fprintf(stderr, "\n%s\n\n", duk_get_string(ctx, -1));
      duk_pop(ctx);
    }
    else {
      fprintf(stderr, "\nThrown Value: %s\n\n", duk_json_encode(ctx, -1));
    }
    duk_destroy_heap(ctx);
    return 1;
  }

  duk_destroy_heap(ctx);
  return 0;
}
