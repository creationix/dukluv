#ifndef DUV_FS_H
#define DUV_FS_H

#include "duv.h"

duk_ret_t duv_fs_close(duk_context *ctx);
duk_ret_t duv_fs_open(duk_context *ctx);
duk_ret_t duv_fs_read(duk_context *ctx);
duk_ret_t duv_fs_unlink(duk_context *ctx);
duk_ret_t duv_fs_write(duk_context *ctx);
duk_ret_t duv_fs_mkdir(duk_context *ctx);
duk_ret_t duv_fs_mkdtemp(duk_context *ctx);
duk_ret_t duv_fs_rmdir(duk_context *ctx);
duk_ret_t duv_fs_scandir(duk_context *ctx);
duk_ret_t duv_fs_scandir_next(duk_context *ctx);
duk_ret_t duv_fs_stat(duk_context *ctx);
duk_ret_t duv_fs_fstat(duk_context *ctx);
duk_ret_t duv_fs_lstat(duk_context *ctx);
duk_ret_t duv_fs_rename(duk_context *ctx);
duk_ret_t duv_fs_fsync(duk_context *ctx);
duk_ret_t duv_fs_fdatasync(duk_context *ctx);
duk_ret_t duv_fs_ftruncate(duk_context *ctx);
duk_ret_t duv_fs_sendfile(duk_context *ctx);
duk_ret_t duv_fs_access(duk_context *ctx);
duk_ret_t duv_fs_chmod(duk_context *ctx);
duk_ret_t duv_fs_fchmod(duk_context *ctx);
duk_ret_t duv_fs_utime(duk_context *ctx);
duk_ret_t duv_fs_futime(duk_context *ctx);
duk_ret_t duv_fs_link(duk_context *ctx);
duk_ret_t duv_fs_symlink(duk_context *ctx);
duk_ret_t duv_fs_readlink(duk_context *ctx);
duk_ret_t duv_fs_chown(duk_context *ctx);
duk_ret_t duv_fs_fchown(duk_context *ctx);

#endif
