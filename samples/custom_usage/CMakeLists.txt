cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

set(OOKII_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(OOKII_GENERATED_OUTPUT "${OOKII_GENERATED_DIR}/custom_usage_generated.cpp")
set(OOKII_GENERATED_INPUT "${CMAKE_CURRENT_LIST_DIR}/arguments.h")

make_directory("${OOKII_GENERATED_DIR}")

# Invoke the PowerShell script to generate the command line parser.
# POWERSHELL_PATH is determined by the top-level CMakeLists.txt
add_custom_command(
    OUTPUT ${OOKII_GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH}
        -ExecutionPolicy Bypass
        "${Ookii.CommandLine.Cpp_SOURCE_DIR}/scripts/New-Parser.ps1"
        -Path "${OOKII_GENERATED_INPUT}"
        -OutputPath "${OOKII_GENERATED_OUTPUT}"
    DEPENDS ${OOKII_GENERATED_INPUT}
    VERBATIM
)

add_executable(custom_usage "main.cpp" "${OOKII_GENERATED_OUTPUT}" "arguments.h")
target_link_libraries(custom_usage PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET custom_usage PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(custom_usage PRIVATE /W4)
else()
  target_compile_options(custom_usage PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  if(NOT OOKIICL_FORCE_LIBFMT)
    check_include_file_cxx("format" HAVE_FORMAT)
  endif()
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(custom_usage PRIVATE fmt::fmt)
  endif()
endif()