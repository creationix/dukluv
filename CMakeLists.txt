cmake_minimum_required(VERSION 2.8.9)

project (dukluv)

include(uv.cmake)
include(duktape.cmake)

add_executable(dukluv
  src/duv.c
  src/main.c
)

target_link_libraries(dukluv duktape uv)
