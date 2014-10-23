#include "duv.h"

int main(int argc, char *argv[]) {
  duk_context *ctx = NULL;
  uv_loop_t loop;
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
    exit(1);
  }
  loop.data = ctx;

  duk_push_global_object(ctx);
  duk_dup(ctx, -1);
  duk_put_prop_string(ctx, -2, "global");
  dukopen_duv(ctx);
  duk_put_prop_string(ctx, -2, "uv");

  duk_eval_file_noresult(ctx, argv[1]);

  duk_destroy_heap(ctx);

  return 0;
}
