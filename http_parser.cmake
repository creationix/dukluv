set(HTTPPARSERDIR ${CMAKE_CURRENT_LIST_DIR}/lib/http-parser)

include_directories(${HTTPPARSERDIR})

add_library(http_parser STATIC
  ${HTTPPARSERDIR}/http_parser.c
)

