#ifndef DUV_MINIZ_H
#define DUV_MINIZ_H

#include "duv.h"

// duk_ret_t duv_mz_reader_init(duk_context *ctx);
// duk_ret_t duv_mz_reader_gc(duk_context *ctx);
// duk_ret_t duv_mz_writer_gc(duk_context *ctx);
// duk_ret_t duv_mz_reader_get_num_files(duk_context *ctx);
// duk_ret_t duv_mz_reader_locate_file(duk_context *ctx);
// duk_ret_t duv_mz_reader_stat(duk_context *ctx);
// duk_ret_t duv_mz_reader_get_filename(duk_context *ctx);
// duk_ret_t duv_mz_reader_is_file_a_directory(duk_context *ctx);
// duk_ret_t duv_mz_reader_extract(duk_context *ctx);
// duk_ret_t duv_mz_reader_get_offset(duk_context *ctx);
// duk_ret_t duv_mz_writer_init(duk_context *ctx);
// duk_ret_t duv_mz_writer_add_from_zip_reader(duk_context *ctx);
// duk_ret_t duv_mz_writer_add_mem(duk_context *ctx);
// duk_ret_t duv_mz_writer_finalize(duk_context *ctx);
duk_ret_t duv_tinfl(duk_context *ctx);
duk_ret_t duv_tdefl(duk_context *ctx);

#endif
