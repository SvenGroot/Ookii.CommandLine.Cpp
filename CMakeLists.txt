cmake_minimum_required (VERSION 3.15)

include(CMakePackageConfigHelpers)

project (
    "Ookii.CommandLine.Cpp"
    VERSION 1.0.0
    HOMEPAGE_URL "https://github.com/SvenGroot/Ookii.CommandLine.Cpp"
    LANGUAGES CXX)

include(GNUInstallDirs)

set(OOKIICL_STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(OOKIICL_STANDALONE_PROJECT ON)
endif()

option(OOKIICL_INSTALL "Generate and install OOKIICL interface library." ${OOKIICL_STANDALONE_PROJECT})
option(OOKIICL_TEST "Build Ookii.CommandLine.Cpp tests." ${OOKIICL_STANDALONE_PROJECT})
option(OOKIICL_SAMPLES "Build Ookii.CommandLine.Cpp samples." ${OOKIICL_STANDALONE_PROJECT})
option(OOKIICL_DOCS "Build Ookii.CommandLine.Cpp documentation." ${OOKIICL_STANDALONE_PROJECT})
option(OOKIICL_PACKAGE "Build Ookii.Command.Cpp NuGet package." ${OOKIICL_STANDALONE_PROJECT})
option(OOKIICL_FORCE_LIBFMT "Force the use of libfmt even if the <format> header is available." OFF)

if (OOKIICL_FORCE_LIBFMT)
    add_compile_definitions(OOKII_FORCE_LIBFMT)
endif()

# Add a header only library, and add an alias so clients can use Ookii.CommandLine::OOKIICL regardless
# of whether they used add_subdirectory or find_package.
add_library(OOKIICL INTERFACE)
add_library(Ookii.CommandLine::OOKIICL ALIAS OOKIICL)

add_subdirectory(include)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/Ookii.CommandLineConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

if (OOKIICL_INSTALL)
    # Since "all" just builds tests and samples, it's not required for install, which just copies the
    # headers.
    #set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
    install(TARGETS OOKIICL EXPORT Ookii.CommandLineConfig)
    install(DIRECTORY include/ookii DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT Ookii.CommandLineConfig NAMESPACE Ookii.CommandLine:: DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/Ookii.CommandLine)
    export(TARGETS OOKIICL NAMESPACE Ookii.CommandLine:: FILE Ookii.CommandLineConfig.cmake)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Ookii.CommandLineConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/Ookii.CommandLine)
endif()

if (OOKIICL_TEST OR OOKIICL_SAMPLES)
    # PowerShell Core or PowerShell 6+ is required to build the "generated" samples, and to run
    # the code-generation script tests.
    find_program(POWERSHELL_PATH NAMES pwsh NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)
    if (POWERSHELL_PATH)
        message("-- Found PowerShell at ${POWERSHELL_PATH}")
    else()
        message(WARNING "-- PowerShell (pwsh) was not found, so the code-generation tests and samples will not be used. See https://github.com/powershell/powershell for instructions on how to install PowerShell for your platform. Note: Windows PowerShell cannot be used; you must install the cross-platform PowerShell.")
    endif()
endif()

if (OOKIICL_TEST)
    enable_testing()
    add_subdirectory("unittests")
    if (POWERSHELL_PATH)
        add_subdirectory("scripts/tests")
    endif()
endif()

if (OOKIICL_SAMPLES)
    add_subdirectory("samples/parser")
    add_subdirectory("samples/subcommand")

    if (POWERSHELL_PATH)
        add_subdirectory("samples/generated_parser")
        add_subdirectory("samples/generated_subcommand")
        add_subdirectory("samples/long_short")
        add_subdirectory("samples/custom_usage")
    endif()
endif()

if (OOKIICL_DOCS)
    add_subdirectory("docs")
endif()

if (OOKIICL_PACKAGE AND POWERSHELL_PATH)
    find_program(NUGET_PATH NAMES NuGet NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)
    if (NUGET_PATH)
        message("-- Found NuGet at ${NUGET_PATH}")
        add_subdirectory("nuget")
    else()
        message(WARNING "-- NuGet was not found, so the NuGet package will not be built.")
    endif()
endif()