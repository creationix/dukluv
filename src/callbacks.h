#ifndef DUV_CALLBACKS_H
#define DUV_CALLBACKS_H

#include "duv.h"

void duv_close_cb(uv_handle_t* handle);
void duv_timer_cb(uv_timer_t* handle);
void duv_connect_cb(uv_connect_t* req, int status);
void duv_shutdown_cb(uv_shutdown_t* req, int status);
void duv_connection_cb(uv_stream_t* handle, int status);
void duv_read_cb(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
void duv_write_cb(uv_write_t* req, int status);
void duv_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

#endif
