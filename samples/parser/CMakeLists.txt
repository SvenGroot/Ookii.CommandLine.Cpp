cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

add_executable(parser "main.cpp" )
target_link_libraries(parser PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET parser PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(parser PRIVATE /W4)
else()
  target_compile_options(parser PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  check_include_file_cxx("format" HAVE_FORMAT)
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(parser PRIVATE fmt::fmt)
  endif()
endif()