set(vw_c_headers
  include/vw/experimental/example.h
  include/vw/experimental/exports.h
  include/vw/experimental/label.h
  include/vw/experimental/io.h
  include/vw/experimental/prediction.h
  include/vw/experimental/types.h
  include/vw/experimental/workspace.h
)

set(vw_c_sources
  src/example.cc
  src/io.cc
)

add_library(vw_c_api ${vw_c_headers} ${vw_c_sources})
target_include_directories(vw_c_api PUBLIC include/)
target_link_libraries(vw_c_api VowpalWabbit::vw VowpalWabbit::io)

add_executable(c_api_test test/test.c)
target_link_libraries(c_api_test PUBLIC vw_c_api)
