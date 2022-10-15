cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

add_executable(unittests "main.cpp" "CommandLineParserTests.cpp"  "LineWrappingStreamTests.cpp" "UobPtrTests.cpp" "ShellCommandTests.cpp" "console.cpp")
target_link_libraries(unittests PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET unittests PROPERTY CXX_STANDARD 20)

add_compile_definitions(OOKII_CONSOLE_NOT_INLINE)

if(MSVC)
  target_compile_options(unittests PRIVATE /W4)

  option(OOKIICL_UNICODE "Compile unit tests with Unicode support" OFF)
  if (OOKIICL_UNICODE)
    add_compile_definitions(_UNICODE UNICODE)
  else ()
    add_compile_definitions(_MBCS)
  endif ()
else()
  target_compile_options(unittests PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  check_include_file_cxx("format" HAVE_FORMAT)
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(unittests PRIVATE fmt::fmt)
  endif()
endif()

add_test(unittests unittests)
