cmake_minimum_required(VERSION 2.8.9)

project (dukluv)

set(DUKTAPEDIR ${CMAKE_CURRENT_LIST_DIR}/lib/duktape)

include_directories(
  ${DUKTAPEDIR}/src
)

include(uv.cmake)

add_executable(dukluv ${DUKTAPEDIR}/src/duktape.c
  src/duv.c
  src/main.c
)

target_link_libraries(dukluv uv m)
