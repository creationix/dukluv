#ifndef DUV_SCHEMA_H
#define DUV_SCHEMA_H

#include "duv.h"

typedef struct {
  const char* name;
  duk_bool_t (*checker)(duk_context *ctx, duk_idx_t index);
} duv_schema_entry;

duk_bool_t duv_is_handle(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_req(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_fs(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_stream(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_timer(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_tcp(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_data(duk_context* ctx, duk_idx_t index);
duk_bool_t duv_is_continuation(duk_context* ctx, duk_idx_t index);
void duv_check_args(duk_context *ctx, const duv_schema_entry schema[]);

#endif
