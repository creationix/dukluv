set(DUKTAPEDIR ${CMAKE_CURRENT_LIST_DIR}/lib/duktape)

include_directories(
  ${DUKTAPEDIR}/src
)

add_library(duktape STATIC ${DUKTAPEDIR}/src/duktape.c)

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
  target_link_libraries(duktape
    m dl
  )
endif()
