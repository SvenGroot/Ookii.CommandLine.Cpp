cmake_minimum_required (VERSION 3.15)

# These tests create files in the "output" directory, which is not under the CMake output path
# because the relative paths to the input header files are part of the test, so this needs to be
# consistent regardless of where CMake's build files are.
make_directory(${CMAKE_CURRENT_LIST_DIR}/output)

add_test(NAME generate-parser-tests
         COMMAND ${POWERSHELL_PATH} ${CMAKE_CURRENT_LIST_DIR}/Run-Test.ps1 ${CMAKE_CURRENT_LIST_DIR}/New-Parser.Tests.ps1)

add_test(NAME generate-subcommand-tests
         COMMAND ${POWERSHELL_PATH} ${CMAKE_CURRENT_LIST_DIR}/Run-Test.ps1 ${CMAKE_CURRENT_LIST_DIR}/New-Subcommand.Tests.ps1)
