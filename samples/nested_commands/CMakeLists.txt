cmake_minimum_required (VERSION 3.15)

include(CheckIncludeFileCXX)
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(OOKII_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(OOKII_GENERATED_OUTPUT "${OOKII_GENERATED_DIR}/nested_commands_generated.cpp")
set(OOKII_GENERATED_INPUT "${CMAKE_CURRENT_LIST_DIR}/base_command.h" "${CMAKE_CURRENT_LIST_DIR}/course_commands.h" "${CMAKE_CURRENT_LIST_DIR}/student_commands.h" "${CMAKE_CURRENT_LIST_DIR}/list_command.h")
list(JOIN OOKII_GENERATED_INPUT "," OOKII_GENERATED_INPUT_LIST)

make_directory("${OOKII_GENERATED_DIR}")

# Invoke the PowerShell script to generate the command line parser.
# POWERSHELL_PATH is determined by the top-level CMakeLists.txt
# We have to use -Command to be able to pass an array
add_custom_command(
    OUTPUT ${OOKII_GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH} -ExecutionPolicy Bypass -Command "&{ ${CMAKE_CURRENT_LIST_DIR}/../../scripts/New-Subcommand.ps1 -Path ${OOKII_GENERATED_INPUT_LIST} -OutputPath ${OOKII_GENERATED_OUTPUT} -NameTransform PascalCase }"
    DEPENDS ${OOKII_GENERATED_INPUT}
    VERBATIM
)

add_executable(nested_commands "main.cpp" ${OOKII_GENERATED_OUTPUT})
target_link_libraries(nested_commands PRIVATE Ookii.CommandLine::OOKIICL nlohmann_json::nlohmann_json)

set_property(TARGET nested_commands PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(nested_commands PRIVATE /W4)
else()
  target_compile_options(nested_commands PRIVATE -Wall -Wextra -Wpedantic)

  # link libfmt if <format> is not supported.
  if(NOT OOKIICL_FORCE_LIBFMT)
    check_include_file_cxx("format" HAVE_FORMAT)
  endif()
  if(NOT HAVE_FORMAT)
    find_package(fmt)
    target_link_libraries(nested_commands PRIVATE fmt::fmt)
  endif()
endif()