cmake_minimum_required(VERSION 2.8.9)

project (dukluv)

include(duktape.cmake)
include(uv.cmake)
include(http_parser.cmake)

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
  src/schema.c
  src/utils.c
)

add_library(dhttp_parser
  src/dhttp_parser.c
)

add_executable(dukluv src/main.c)

target_link_libraries(dukluv duv dhttp_parser duktape uv http_parser)
