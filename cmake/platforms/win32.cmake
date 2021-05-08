# This file contains header definitions applicable to Win32
# CMAKE_SYSTEM_VERSION must come before the project is defined in the top level CMakeLists file
# https://stackoverflow.com/questions/45692367/how-to-set-msvc-target-platform-version-with-cmake


if(WIN32)
  # VW targets Windows 10.0.16299.0 SDK
  set(CMAKE_SYSTEM_VERSION "10.0.16299.0" CACHE INTERNAL "Windows SDK version to target.")
else()
  message(FATAL_ERROR "Loading Win32-specific configuration under a non-Win32 build.")
endif()