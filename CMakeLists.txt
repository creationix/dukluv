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
  src/miniz.c
)


add_library(dschema
  src/schema.c
)

add_executable(dukluv src/main.c)

target_link_libraries(dukluv duv dschema duktape uv)

set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
SET (BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE STRING "Directory where lib will install")
SET (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Directory where lib will install")
SET (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")

install(TARGETS duv dschema duktape uv DESTINATION ${LIB_INSTALL_DIR})
install(TARGETS dukluv DESTINATION ${BIN_INSTALL_DIR})
install(FILES src/refs.h src/duv.h lib/duktape/src-separate/duk_config.h lib/duktape/src-separate/duktape.h DESTINATION ${INCLUDE_INSTALL_DIR}/duv)
