cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

add_executable(shell_command "main.cpp" "read_command.cpp" "read_command.h" "write_command.h" "write_command.cpp")
target_link_libraries(shell_command PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET shell_command PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(shell_command PRIVATE /W4)
else()
  target_compile_options(shell_command PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  check_include_file_cxx("format" HAVE_FORMAT)
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(shell_command PRIVATE fmt::fmt)
  endif()
endif()