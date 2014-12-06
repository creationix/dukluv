#include "fs.h"

static void duv_push_timespec_table(duk_context *ctx, const uv_timespec_t* t) {
  duk_push_object(ctx);
  duk_push_uint(ctx, t->tv_sec);
  duk_put_prop_string(ctx, -2, "sec");
  duk_push_uint(ctx, t->tv_nsec);
  duk_put_prop_string(ctx, -2, "nsec");
}

static void duv_push_stats_table(duk_context *ctx, const uv_stat_t* s) {
  const char* type = NULL;
  duk_push_object(ctx);
  duk_push_uint(ctx, s->st_mode);
  duk_put_prop_string(ctx, -2, "mode");
  duk_push_uint(ctx, s->st_uid);
  duk_put_prop_string(ctx, -2, "uid");
  duk_push_uint(ctx, s->st_gid);
  duk_put_prop_string(ctx, -2, "gid");
  duk_push_uint(ctx, s->st_size);
  duk_put_prop_string(ctx, -2, "size");
  duv_push_timespec_table(ctx, &s->st_atim);
  duk_put_prop_string(ctx, -2, "atime");
  duv_push_timespec_table(ctx, &s->st_mtim);
  duk_put_prop_string(ctx, -2, "mtime");
  duv_push_timespec_table(ctx, &s->st_ctim);
  duk_put_prop_string(ctx, -2, "ctime");
  if (S_ISREG(s->st_mode)) {
    type = "file";
  }
  else if (S_ISDIR(s->st_mode)) {
    type = "directory";
  }
  else if (S_ISLNK(s->st_mode)) {
    type = "link";
  }
  else if (S_ISFIFO(s->st_mode)) {
    type = "fifo";
  }
#ifdef S_ISSOCK
  else if (S_ISSOCK(s->st_mode)) {
    type = "socket";
  }
#endif
  else if (S_ISCHR(s->st_mode)) {
    type = "char";
  }
  else if (S_ISBLK(s->st_mode)) {
    type = "block";
  }
  if (type) {
  duk_push_string(ctx, type);
  duk_put_prop_string(ctx, -2, "type");
  }
}

static int duv_string_to_flags(duk_context *ctx, const char* string) {
  bool read = false;
  bool write = false;
  int flags = 0;
  while (string) {
    switch (string[0]) {
      case 'r': read = true; break;
      case 'w': write = true; flags |= O_TRUNC | O_CREAT; break;
      case 'a': write = true; flags |= O_APPEND | O_CREAT; break;
      case '+': read = true; write = true; break;
      case 'x': flags |= O_EXCL; break;
      case 's': flags |= O_SYNC; break;
      default:
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "Unknown file open flag '%s'", string);
        return 0;
    }
    string++;
  }
  flags |= read ? (write ? O_RDWR : O_RDONLY) :
                  (write ? O_WRONLY : 0);
  return flags;
}

static void duv_push_error_result(duk_context *ctx, uv_fs_t* req) {
  if (req->path) {
    duk_push_error_object(ctx, DUK_ERR_ERROR, "%s: %s: %s", uv_err_name(req->result), uv_strerror(req->result), req->path);
  }
  else {
    duk_push_error_object(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(req->result), uv_strerror(req->result));
  }
}

/* Processes a result and pushes the data onto the stack
   returns the number of items pushed */
static void duv_push_fs_result(duk_context *ctx, uv_fs_t* req) {
  duv_req_t* data = req->data;
  char* chunk;

  switch (req->fs_type) {
    case UV_FS_CLOSE:
    case UV_FS_RENAME:
    case UV_FS_UNLINK:
    case UV_FS_RMDIR:
    case UV_FS_MKDIR:
    case UV_FS_FTRUNCATE:
    case UV_FS_FSYNC:
    case UV_FS_FDATASYNC:
    case UV_FS_LINK:
    case UV_FS_SYMLINK:
    case UV_FS_CHMOD:
    case UV_FS_FCHMOD:
    case UV_FS_CHOWN:
    case UV_FS_FCHOWN:
    case UV_FS_UTIME:
    case UV_FS_FUTIME:
      duk_push_boolean(ctx, 1);
      break;

    case UV_FS_OPEN:
    case UV_FS_SENDFILE:
    case UV_FS_WRITE:
      duk_push_uint(ctx, req->result);
      break;

    case UV_FS_STAT:
    case UV_FS_LSTAT:
    case UV_FS_FSTAT:
      duv_push_stats_table(ctx, &req->statbuf);
      break;

    case UV_FS_READLINK:
      duk_push_string(ctx, (char*)req->ptr);
      break;

    case UV_FS_MKDTEMP:
      duk_push_string(ctx, req->path);
      break;

    case UV_FS_READ:
      chunk = duk_push_fixed_buffer(ctx, req->result);
      memcpy(chunk, data->data, req->result);
      break;

    case UV_FS_SCANDIR:
      // Expose the userdata for the request.
      duv_push_ref(ctx, data->req_ref);
      break;

    default:
      duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "UNKNOWN FS TYPE %d\n", req->fs_type);
      break;
  }

}

static void duv_fs_cb(uv_fs_t* req) {
  duk_context *ctx = req->loop->data;
  int nargs;

  if (req->result < 0) {
    duk_push_null(ctx);
    duv_push_error_result(ctx, req);
    nargs = 2;
  }
  else {
    duv_push_fs_result(ctx, req);
    nargs = 1;
  }
  duv_fulfill_req(ctx, (uv_req_t*)req, nargs);
  if (req->fs_type != UV_FS_SCANDIR) {
    req->data = duv_cleanup_req(ctx, req->data);
    uv_fs_req_cleanup(req);
  }
}

#define FS_CALL(func, req, ...) {                         \
  int ret, sync;                                          \
  duv_req_t* data = req->data;                            \
  sync = data->callback_ref == 0;                         \
  ret = uv_fs_##func(duv_loop(ctx), req, __VA_ARGS__,     \
                     sync ? NULL : duv_fs_cb);            \
  if (sync || ret < 0) {                                  \
    if (req->result < 0) {                                \
      duv_push_error_result(ctx, req);                    \
    }                                                     \
    else {                                                \
      duv_push_fs_result(ctx, req);                       \
    }                                                     \
    if (req->fs_type != UV_FS_SCANDIR) {                  \
      duv_cleanup_req(ctx, req->data);                    \
      req->data = NULL;                                   \
      uv_fs_req_cleanup(req);                             \
    }                                                     \
    if (req->result < 0) {                                \
      duk_throw(ctx);                                     \
    }                                                     \
    return 1;                                             \
  }                                                       \
  duv_push_ref(ctx, data->req_ref);                       \
  return 1;                                               \
}

duk_ret_t duv_fs_close(duk_context *ctx) {
  uv_file file = duk_require_int(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(close, req, file);
}

duk_ret_t duv_fs_open(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  int flags = duv_string_to_flags(ctx, duk_require_string(ctx, 1));
  int mode = duk_require_uint(ctx, 2);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);
  FS_CALL(open, req, path, flags, mode);
}

duk_ret_t duv_fs_read(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  int64_t len = duk_require_uint(ctx, 1);
  int64_t offset = duk_require_uint(ctx, 2);
  uv_buf_t buf;
  uv_fs_t* req;
  buf = uv_buf_init(duk_alloc(ctx, len), len);
  req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);
  // TODO: find out why we can't just use req->ptr for the base
  ((duv_req_t*)req->data)->data = buf.base;
  FS_CALL(read, req, file, &buf, 1, offset);
}

duk_ret_t duv_fs_unlink(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(unlink, req, path);
}

duk_ret_t duv_fs_write(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  uv_buf_t buf;
  int64_t offset;
  uv_fs_t* req;
  buf.base = (char*)duk_require_lstring(ctx, 1, &buf.len);
  offset = duk_require_uint(ctx, 2);
  req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);
  req->ptr = buf.base;
  FS_CALL(write, req, file, &buf, 1, offset);
}

duk_ret_t duv_fs_mkdir(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  int mode = duk_require_uint(ctx, 1);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 2);
  FS_CALL(mkdir, req, path, mode);
}

duk_ret_t duv_fs_mkdtemp(duk_context *ctx) {
  const char* tpl = duk_require_string(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(mkdtemp, req, tpl);
}

duk_ret_t duv_fs_rmdir(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(rmdir, req, path);
}

duk_ret_t duv_fs_scandir(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  int flags = 0; // TODO: find out what these flags are.
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(scandir, req, path, flags);
}

duk_ret_t duv_fs_scandir_next(duk_context *ctx) {
  uv_fs_t* req;
  uv_dirent_t ent;
  int ret;
  const char* type;

  dschema_check(ctx, (const duv_schema_entry[]) {
    {"req", duv_is_fs},
    {NULL}
  });

  req = duk_get_buffer(ctx, 0, NULL);
  ret = uv_fs_scandir_next(req, &ent);
  if (ret == UV_EOF) {
    duv_cleanup_req(ctx, req->data);
    req->data = NULL;
    uv_fs_req_cleanup(req);
    return 0;
  }
  if (ret < 0) duv_error(ctx, ret);
  duk_push_object(ctx);
  duk_push_string(ctx, ent.name);
  duk_put_prop_string(ctx, -2, "name");
  switch (ent.type) {
    case UV_DIRENT_UNKNOWN: type = NULL;     break;
    case UV_DIRENT_FILE:    type = "file";   break;
    case UV_DIRENT_DIR:     type = "directory";    break;
    case UV_DIRENT_LINK:    type = "link";   break;
    case UV_DIRENT_FIFO:    type = "fifo";   break;
    case UV_DIRENT_SOCKET:  type = "socket"; break;
    case UV_DIRENT_CHAR:    type = "char";   break;
    case UV_DIRENT_BLOCK:   type = "block";  break;
  }
  if (type) {
    duk_push_string(ctx, type);
    duk_put_prop_string(ctx, -2, "type");
  }
  return 1;
}

duk_ret_t duv_fs_stat(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(stat, req, path);
}

duk_ret_t duv_fs_fstat(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(fstat, req, file);
}

duk_ret_t duv_fs_lstat(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(lstat, req, path);
}

duk_ret_t duv_fs_rename(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  const char* new_path = duk_require_string(ctx, 1);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 2);
  FS_CALL(rename, req, path, new_path);
}

duk_ret_t duv_fs_fsync(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(fsync, req, file);
}

duk_ret_t duv_fs_fdatasync(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(fdatasync, req, file);
}

duk_ret_t duv_fs_ftruncate(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  int64_t offset = duk_require_uint(ctx, 1);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 2);
  FS_CALL(ftruncate, req, file, offset);
}

duk_ret_t duv_fs_sendfile(duk_context *ctx) {
  uv_file out_fd = duk_require_uint(ctx, 0);
  uv_file in_fd = duk_require_uint(ctx, 1);
  int64_t in_offset = duk_require_uint(ctx, 2);
  size_t length = duk_require_uint(ctx, 3);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 4);
  FS_CALL(sendfile, req, out_fd, in_fd, in_offset, length);
}

duk_ret_t duv_fs_access(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  const char* string = duk_require_string(ctx, 1);
  int mode = 0;
  size_t i, l;
  for (i = 0, l = strlen(string); i < l; ++i) {
    switch (string[i]) {
      case 'r': case 'R': mode |= R_OK; break;
      case 'w': case 'W': mode |= W_OK; break;
      case 'x': case 'X': mode |= X_OK; break;
      default:
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "Unknown file access mode '%s'", string);
        return 0;
    }
  }
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 2);
  FS_CALL(access, req, path, mode);
}

duk_ret_t duv_fs_chmod(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  int mode = duk_require_uint(ctx, 1);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 2);
  FS_CALL(chmod, req, path, mode);
}

duk_ret_t duv_fs_fchmod(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  int mode = duk_require_uint(ctx, 1);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 2);
  FS_CALL(fchmod, req, file, mode);
}

duk_ret_t duv_fs_utime(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  double atime = duk_require_number(ctx, 1);
  double mtime = duk_require_number(ctx, 2);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);
  FS_CALL(utime, req, path, atime, mtime);
}

duk_ret_t duv_fs_futime(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  double atime = duk_require_number(ctx, 1);
  double mtime = duk_require_number(ctx, 2);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);
  FS_CALL(futime, req, file, atime, mtime);
}

duk_ret_t duv_fs_link(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  const char* new_path = duk_require_string(ctx, 1);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 2);
  FS_CALL(link, req, path, new_path);
}

duk_ret_t duv_fs_symlink(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  const char* new_path = duk_require_string(ctx, 1);
  int flags = 0;
  uv_fs_t* req;
  if (duk_is_object(ctx, 2)) {
    duk_get_prop_string(ctx, 2, "dir");
    if (duk_get_boolean(ctx, -1)) flags |= UV_FS_SYMLINK_DIR;
    duk_pop(ctx);
    duk_get_prop_string(ctx, 2, "junction");
    if (duk_get_boolean(ctx, -1)) flags |= UV_FS_SYMLINK_JUNCTION;
    duk_pop(ctx);
  }
  req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);

  FS_CALL(symlink, req, path, new_path, flags);
}

duk_ret_t duv_fs_readlink(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 1);
  FS_CALL(readlink, req, path);
}

duk_ret_t duv_fs_chown(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  uv_uid_t uid = duk_require_uint(ctx, 1);
  uv_uid_t gid = duk_require_uint(ctx, 2);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);
  FS_CALL(chown, req, path, uid, gid);
}

duk_ret_t duv_fs_fchown(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  uv_uid_t uid = duk_require_uint(ctx, 1);
  uv_uid_t gid = duk_require_uint(ctx, 2);
  uv_fs_t* req = duk_push_fixed_buffer(ctx, sizeof(*req));
  req->data = duv_setup_req(ctx, 3);
  FS_CALL(fchown, req, file, uid, gid);
}
