#ifndef DSCHEMA_H
#define DSCHEMA_H

#include "duktape.h"

typedef struct {
  const char* name;
  duk_bool_t (*checker)(duk_context *ctx, duk_idx_t index);
} duv_schema_entry;

duk_bool_t dschema_is_data(duk_context* ctx, duk_idx_t index);
duk_bool_t dschema_is_continuation(duk_context* ctx, duk_idx_t index);
void dschema_check(duk_context *ctx, const duv_schema_entry schema[]);

#endif
