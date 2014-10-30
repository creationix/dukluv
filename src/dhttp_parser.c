#include "dhttp_parser.h"

static int on_message_begin(http_parser* _) {
  (void)_;
  printf("\n***MESSAGE BEGIN***\n\n");
  return 0;
}

static int on_headers_complete(http_parser* _) {
  (void)_;
  printf("\n***HEADERS COMPLETE***\n\n");
  return 0;
}

static int on_message_complete(http_parser* _) {
  (void)_;
  printf("\n***MESSAGE COMPLETE***\n\n");
  return 0;
}

static int on_url(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Url: %.*s\n", (int)length, at);
  return 0;
}

static int on_header_field(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Header field: %.*s\n", (int)length, at);
  return 0;
}

static int on_header_value(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Header value: %.*s\n", (int)length, at);
  return 0;
}

static int on_body(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Body: %.*s\n", (int)length, at);
  return 0;
}


static duk_int_t dhttp_parser_version(duk_context *ctx) {
  unsigned long version;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {NULL}
  });

  version = http_parser_version();
  duk_push_uint(ctx, version);
  return 1;
}

static duk_int_t dhttp_new_parser(duk_context *ctx) {
  enum http_parser_type file_type;

  const char* type = duk_require_string(ctx, 0);

  if (strcmp(type, "REQUEST") == 0) {
    file_type = HTTP_REQUEST;
  }
  else if (strcmp(type, "RESPONSE") == 0) {
    file_type = HTTP_RESPONSE;
  }
  else if (strcmp(type, "BOTH") == 0) {
    file_type = HTTP_BOTH;
  }
  else {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "parser type must be 'REQUEST', 'RESPONSE', or 'BOTH'");
  }

  http_parser_settings settings;
  memset(&settings, 0, sizeof(settings));
  settings.on_message_begin = on_message_begin;
  settings.on_url = on_url;
  settings.on_header_field = on_header_field;
  settings.on_header_value = on_header_value;
  settings.on_headers_complete = on_headers_complete;
  settings.on_body = on_body;
  settings.on_message_complete = on_message_complete;

  http_parser parser;
  http_parser_init(&parser, file_type);
// void http_parser_init(http_parser *parser, enum http_parser_type type);
}
static duk_int_t dhttp_parser_execute(duk_context *ctx) {
// size_t http_parser_execute(http_parser *parser,
}
static duk_int_t dhttp_should_keep_alive(duk_context *ctx) {
// int http_should_keep_alive(const http_parser *parser);
}
static duk_int_t dhttp_parser_parse_url(duk_context *ctx) {
  // int http_parser_parse_url(const char *buf, size_t buflen,
}
static duk_int_t dhttp_parser_pause(duk_context *ctx) {
  // void http_parser_pause(http_parser *parser, int paused);
}
static duk_int_t dhttp_body_is_final(duk_context *ctx) {
  // int http_body_is_final(const http_parser *parser);
}

static const duk_function_list_entry dhttp_parser_funcs[] = {
  {"parser_version", dhttp_parser_version},
  {"new_parser", dhttp_new_parser},
  {"parser_execute", dhttp_parser_execute},
  {"should_keep_alive", dhttp_should_keep_alive},
  {"parse_url", dhttp_parser_parse_url},
  {"pause", dhttp_parser_pause},
  {"body_is_final", dhttp_body_is_final},
  {NULL, NULL, 0}
};

duk_ret_t dukopen_http_parser(duk_context *ctx) {
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, dhttp_parser_funcs);
  return 1;
}
