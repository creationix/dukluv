#include "uv_schema.h"

duk_bool_t duv_is_fd(duk_context* ctx, duk_idx_t index) {
  return duk_is_number(ctx, index);
}
