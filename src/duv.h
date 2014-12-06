#ifndef DUV_H
#define DUV_H

#include "duktape.h"
#include "uv.h"
#include <assert.h>
#include <unistd.h>

typedef enum { false, true } bool;

#if defined(_WIN32)
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# define S_ISREG(x)  (((x) & _S_IFMT) == _S_IFREG)
# define S_ISDIR(x)  (((x) & _S_IFMT) == _S_IFDIR)
# define S_ISFIFO(x) (((x) & _S_IFMT) == _S_IFIFO)
# define S_ISCHR(x)  (((x) & _S_IFMT) == _S_IFCHR)
# define S_ISBLK(x)  0
# define S_ISLNK(x)  0
# define S_ISSOCK(x) 0
#endif

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
  int context;
  int callbacks[2];
} duv_handle_t;

typedef struct {
  int req_ref; // ref for uv_req_t's userdata
  int context;
  int callback_ref; // ref for callback
  void* data; // extra data
} duv_req_t;

duk_ret_t dukopen_uv(duk_context *ctx);

#include "refs.h"
#include "utils.h"
#include "schema.h"
#include "uv_schema.h"

#endif
