param(
    [Parameter()][string]$OutputPath,
    [Parameter()][string]$NuGetPath = "nuget.exe"
)

$packageSource = Join-Path $PSScriptRoot "package"
Copy-Item $packageSource $OutputPath -Recurse -Force

$parentPath = Split-Path $PSScriptRoot -Parent
$packageDest = Join-Path $OutputPath "package"
$includeSource = Join-Path $parentPath "include" "ookii" "*.h"
$includeDest = Join-Path $packageDest "include" "ookii"
New-Item -ItemType Directory $includeDest -Force | Out-Null
Copy-Item $includeSource $includeDest -Filter "*.h" -Force

$scriptSource = Join-Path $parentPath "scripts" "*.ps1"
$scriptDest = Join-Path $packageDest "tools"
New-Item -ItemType Directory $scriptDest -Force | Out-Null
Copy-Item $scriptSource $scriptDest -Force

$commit = git rev-parse HEAD
$nuspec = "Ookii.CommandLine.Cpp.nuspec"
$nuspecSource = Join-Path $packageSource $nuspec
$nuspecDest = Join-Path $packageDest $nuspec
Get-Content $nuspecSource | ForEach-Object {
    $_.Replace("%COMMIT%", $commit)
    
} | Set-Content $nuspecDest

# Included scripts are not intended to be run during package installation so suppress that warning.
&$NuGetPath pack $nuspecDest -OutputDirectory $OutputPath -Properties "NoWarn=NU5111"