cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

set(OOKII_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(OOKII_GENERATED_OUTPUT "${OOKII_GENERATED_DIR}/long_short_generated.cpp")
set(OOKII_GENERATED_INPUT "${CMAKE_CURRENT_LIST_DIR}/arguments.h")

make_directory("${OOKII_GENERATED_DIR}")

# Invoke the PowerShell script to generate the command line parser.
# POWERSHELL_PATH is determined by the top-level CMakeLists.txt
add_custom_command(
    OUTPUT ${OOKII_GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH} -ExecutionPolicy Bypass "${CMAKE_CURRENT_LIST_DIR}/../../scripts/New-Parser.ps1" -Path "${OOKII_GENERATED_INPUT}" -OutputPath "${OOKII_GENERATED_OUTPUT}" -EntryPoint "ookii_main"
    DEPENDS ${OOKII_GENERATED_INPUT}
    VERBATIM
)

add_executable(long_short "main.cpp" "${OOKII_GENERATED_OUTPUT}" "arguments.h")
target_link_libraries(long_short PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET long_short PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(long_short PRIVATE /W4)
else()
  target_compile_options(long_short PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  if(NOT OOKIICL_FORCE_LIBFMT)
    check_include_file_cxx("format" HAVE_FORMAT)
  endif()
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(long_short PRIVATE fmt::fmt)
  endif()
endif()