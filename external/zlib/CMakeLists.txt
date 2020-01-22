cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  extern_zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        v1.2.11
)

FetchContent_GetProperties(extern_zlib)
if(NOT extern_zlib_POPULATED)
  FetchContent_Populate(extern_zlib)
  add_subdirectory(
    ${extern_zlib_SOURCE_DIR}
    ${extern_zlib_BINARY_DIR}
    EXCLUDE_FROM_ALL
  )
endif()

target_include_directories(zlib PUBLIC ${extern_zlib_SOURCE_DIR} ${extern_zlib_BINARY_DIR})
add_library(ZLIB::ZLIB ALIAS zlib)