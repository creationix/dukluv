#include "duktape.h"
#include "uv.h"

int main(int argc, const char *argv[]) {
  duk_context *ctx = NULL;

  if (argc < 2) {
    fprintf(stderr, "Usage: dukluv script.js\n");
    exit(1);
  }

  ctx = duk_create_heap_default();
  if (!ctx) {
    fprintf(stderr, "Problem initiailizing duktape heap\n");
    exit(1);
  }
  duk_eval_file_noresult(ctx, argv[1]);

  return 0;
}
