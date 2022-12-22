cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

set(SRC_FILES "main.cpp" "CommandLineParserTests.cpp"  "LineWrappingStreamTests.cpp" "SubcommandTests.cpp" "console.cpp")

if (WIN32)
    list(APPEND SRC_FILES "unittests.rc")
endif()

add_executable(unittests ${SRC_FILES})
target_link_libraries(unittests PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET unittests PROPERTY CXX_STANDARD 20)

add_compile_definitions(OOKII_PLATFORM_NOT_INLINE)

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
  if(NOT OOKIICL_FORCE_LIBFMT)
    check_include_file_cxx("format" HAVE_FORMAT)
  endif()
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(unittests PRIVATE fmt::fmt)
  endif()
endif()

add_test(unittests unittests)
