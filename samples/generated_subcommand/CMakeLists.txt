cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)

set(OOKII_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(OOKII_GENERATED_OUTPUT "${OOKII_GENERATED_DIR}/commands_generated.cpp")
set(OOKII_GENERATED_INPUT "${CMAKE_CURRENT_LIST_DIR}/global.h" "${CMAKE_CURRENT_LIST_DIR}/read_command.h" "${CMAKE_CURRENT_LIST_DIR}/write_command.h")
list(JOIN OOKII_GENERATED_INPUT "," OOKII_GENERATED_INPUT_LIST)

make_directory("${OOKII_GENERATED_DIR}")

# Invoke the PowerShell script to generate the command line parser.
# POWERSHELL_PATH is determined by the top-level CMakeLists.txt
# We have to use -Command to be able to pass an array
add_custom_command(
    OUTPUT ${OOKII_GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH} -ExecutionPolicy Bypass -Command "&{ ${CMAKE_CURRENT_LIST_DIR}/../../scripts/New-Subcommand.ps1 -Path ${OOKII_GENERATED_INPUT_LIST} -OutputPath ${OOKII_GENERATED_OUTPUT} -GenerateMain }"
    DEPENDS ${OOKII_GENERATED_INPUT}
    VERBATIM
)

add_executable(generated_subcommand "read_command.cpp" "read_command.h" "write_command.h" "write_command.cpp" ${OOKII_GENERATED_OUTPUT})
target_link_libraries(generated_subcommand PRIVATE Ookii.CommandLine::OOKIICL)

set_property(TARGET generated_subcommand PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(generated_subcommand PRIVATE /W4)
else()
  target_compile_options(generated_subcommand PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  if(NOT OOKIICL_FORCE_LIBFMT)
    check_include_file_cxx("format" HAVE_FORMAT)
  endif()
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(generated_subcommand PRIVATE fmt::fmt)
  endif()
endif()