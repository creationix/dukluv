#ifndef DUV_H
#define DUV_H

#include "duktape.h"
#include "uv.h"
#include "refs.h"
#include "utils.h"

#ifndef PATH_MAX
#define PATH_MAX (8096)
#endif

#ifndef MAX_TITLE_LENGTH
#define MAX_TITLE_LENGTH (8192)
#endif

// There are two slots for holding callbacks.  One is for the CLOSED event.
// The other slot is for all others since they never conflict in practice.
#define duv_callback_id int
#define DUV_CLOSED 0
#define DUV_TIMEOUT 1
#define DUV_PREPARE 1
#define DUV_IDLE 1
#define DUV_CHECK 1
#define DUV_ASYNC 1
#define DUV_POLL 1
#define DUV_SIGNAL 1
#define DUV_EXIT 1
#define DUV_CONNECTION 1
#define DUV_READ 1
#define DUV_RECV 1
#define DUV_FS_EVENT 1
#define DUV_FS_POLL 1

// Ref for userdata and event callbacks
typedef struct {
  int ref;
  int callbacks[2];
} duv_handle_t;

duk_ret_t dukopen_duv(duk_context *ctx);

// From misc.c
static const char* duv_protocol_to_string(int family);

#endif
