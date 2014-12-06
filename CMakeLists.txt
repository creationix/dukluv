cmake_minimum_required(VERSION 2.8.9)

project (dukluv)

include(duktape.cmake)
include(uv.cmake)

add_library(duv
  src/duv.c
  src/refs.c
  src/utils.c
  src/loop.c
  src/req.c
  src/handle.c
  src/timer.c
  src/stream.c
  src/tcp.c
  src/pipe.c
  src/tty.c
  src/fs.c
  src/misc.c
  src/utils.c
  src/uv_schema.c
  src/callbacks.c
)

add_library(dschema
  src/schema.c
)

add_executable(dukluv src/main.c)

target_link_libraries(dukluv duv dschema duktape uv)
