#include "req.h"

duk_ret_t duv_cancel(duk_context *ctx) {
  uv_req_t* req;

  duv_check_args(ctx, (const duv_schema_entry[]) {
    {"req", duv_is_req},
    {NULL}
  });

  req = duk_get_buffer(ctx, 0, NULL);
  duv_check(ctx, uv_cancel(req));
  return 0;
}
