# Invoke the PowerShell script to generate a NuGet package.
add_custom_target(
    nuget ALL
    COMMAND ${POWERSHELL_PATH} -ExecutionPolicy Bypass "${CMAKE_CURRENT_LIST_DIR}/New-NuGetPackage.ps1" -OutputPath "${CMAKE_CURRENT_BINARY_DIR}"
    VERBATIM
)
