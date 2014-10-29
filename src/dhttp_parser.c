#include "dhttp_parser.h"

static const duk_function_list_entry dhttp_parser_funcs[] = {
  {NULL, NULL, 0}
};

duk_ret_t dukopen_http_parser(duk_context *ctx) {
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, dhttp_parser_funcs);
  return 1;
}
