#ifndef DHTTP_PARSER_H
#define DHTTP_PARSER_H

#include <assert.h>
#include "duktape.h"
#include "http_parser.h"

duk_ret_t dukopen_http_parser(duk_context *ctx);

#endif
