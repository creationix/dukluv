#include "schema.h"

duk_bool_t dschema_is_data(duk_context* ctx, duk_idx_t index) {
  return duk_is_string(ctx, index) || duk_is_buffer(ctx, index);
}

duk_bool_t dschema_is_continuation(duk_context* ctx, duk_idx_t index) {
  return !duk_is_valid_index(ctx, index) ||
          duk_is_function(ctx, index) ||
          duk_is_undefined(ctx, index);
}

void dschema_check(duk_context *ctx, const duv_schema_entry schema[]) {
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
