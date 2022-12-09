cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

add_executable(subcommand "main.cpp" "read_command.cpp" "read_command.h" "write_command.h" "write_command.cpp")
target_link_libraries(subcommand PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET subcommand PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(subcommand PRIVATE /W4)
else()
  target_compile_options(subcommand PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  if(NOT OOKIICL_FORCE_LIBFMT)
    check_include_file_cxx("format" HAVE_FORMAT)
  endif()
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(subcommand PRIVATE fmt::fmt)
  endif()
endif()