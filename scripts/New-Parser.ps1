#Requires -PSEdition Core

<#
    .SYNOPSIS

        Generates a command line argument parser from annotated C++ source code.

    .DESCRIPTION

        This script takes one or more C++ header files, and parses them for specially annotated
        structs or classes that define command line arguments.

        It will generate a method that creates a command_line_parser for those arguments, parses
        them, and prints error messages and usage help if an error occurred.

        Optionally, it can also generate a main() method for your application, which parses the
        arguments and invokes a specified method with the result.

        For information on how to annotate your struct or class to generate arguments, refer to
        the Ookii.CommandLine.Cpp documentation.

        If multiple arguments types are found, parsers are generated for all of them. In that case,
        no main method can be generated.

#>
param(
    # Specifies the path to the C++ header file to process. Wildcards are permitted.
    [Parameter(Mandatory=$true, Position=0, ParameterSetName="Path", ValueFromPipeline=$true, ValueFromPipelineByPropertyName=$true)]
    [ValidateNotNullOrEmpty()]
    [SupportsWildcards()]
    [string[]] $Path,
    # Specifies the path to the C++ header file to process. Unlike the Path parameter, the value of
    # the LiteralPath parameter is used exactly as it is typed. No characters are interpreted as
    # wildcards. If the path includes escape characters, enclose it in single quotation marks.
    # Single quotation marks tell Windows PowerShell not to# interpret any characters as escape
    # sequences.
    [Parameter(Mandatory=$true, ParameterSetName="LiteralPath", ValueFromPipelineByPropertyName=$true)]
    [Alias("PSPath")]
    [ValidateNotNullOrEmpty()]
    [string[]] $LiteralPath,
    # Specifies the path to the generated file to create. If this file already exists, it will be
    # overwritten.
    [Parameter(Mandatory=$true, Position=1)]
    [ValidateNotNullOrEmpty()]
    [string]$OutputPath,
    # Specifies how field names should be changed to derive an argument name.
    #
    # Possible values:
    # - None: use the field name as is; this is the default.
    # - PascalCase: transform the field names to PascalCase; e.g. "arg_name" becomes "ArgName".
    # - CamelCase: transform the field name to camelCase; e.g. "arg_name" becomes "argName".
    # - SnakeCase: transform the field name to snake_case; e.g. "argName" becomes "arg_name".
    # - DashCase: transform the field name to dash-case; e.g. "argName" or "arg_name" becomes "arg-name".
    # - Trim: only trim leading and trailing underscores.
    #
    # All options except None will trim leading and trailing underscores.
    #
    # This does not affect explicitly specified argument names.
    [Parameter()]
    [ValidateSet("None", "PascalCase", "CamelCase", "SnakeCase", "DashCase", "Trim")]
    [string]$NameTransform = "None",
    # Indicates that the generated code should use wide characters. If specified, the code will use
    # "wchar_t" instead of "char", string literals are prefixed with L"", and if the main method is
    # generated, it will be called "wmain" instead of "main".
    [Parameter()][switch]$WideChar,
    # Supplies the name of an entry point method. When specified, a main method will be generated
    # that parses the arguments, and calls the specified method if successfull, passing the
    # arguments as the only parameter. The method will not be called if argument parsing failed.
    # If not specified, no main method is generated.
    [Parameter()][string]$EntryPoint,
    # Supplies additional header files that should be included in the generated result file before
    # any other header files. Use this to include, for example, a precompiled header file. Note
    # that the provided values are used as is, so the path must be relative to the generated output
    # or the file must be on the include path. Usually, it's easier to include required headers
    # from the header passed to -Path or -LiteralPath.
    [Parameter()][string[]]$AdditionalHeaders = @()
)
begin {
    . (Join-Path $PSScriptRoot common.ps1)
    $outputDir = Split-Path -LiteralPath $OutputPath
    if (-not $outputDir) {
        $outputDir = "."
    }

    $outputDir = Convert-Path -LiteralPath $outputDir
    [string[]]$headers = $AdditionalHeaders | ForEach-Object {
        "#include `"$_`""
    }

    $headers += "#include <filesystem>",
        "#include <ookii/command_line.h>"

    if ($WideChar) {
        $stringPrefix = "L"
        $charType = "wchar_t"
        $mainName = "wmain"
    } else {
        $stringPrefix = ""
        $charType = "char"
        $mainName = "main"
    }

    $infoCount = 0
}
process {
    if ($Path) {
        $inputs = Get-Item -Path $Path
    } else {
        $inputs = Get-Item -LiteralPath $LiteralPath
    }

    foreach ($file in $inputs)
    {
        $contents = $file | Get-Content
        # Get the relative path to the header from the generated output file, and always use / even on Windows.
        $fileName = [System.IO.Path]::GetRelativePath($outputDir, $file.FullName).Replace([System.IO.Path]::DirectorySeparatorChar, [System.IO.Path]::AltDirectorySeparatorChar)
        $headers += "#include `"$fileName`""
        foreach ($info in (Convert-Arguments $contents)) {
            $infoCount += 1
            $result += "std::optional<$($info.TypeName)> $($info.TypeName)::parse(int argc, const $charType *const argv[], ookii::basic_usage_writer<$chartype> *options, ookii::basic_localized_string_provider<$chartype> *string_provider)
{
$($info.GenerateParser($stringPrefix, $charType, $NameTransform) -join [System.Environment]::NewLine)
}

"
        }
    }
}
end {
    if ($infoCount -eq 0) {
        throw "No arguments types found."
    }

    if ($EntryPoint) {
        if ($infoCount -gt 1) {
            throw "Can't generate entry point with more than one arguments type."
        }

        $result += "int $mainName(int argc, $charType *argv[])
{
    auto args = $($info.TypeName)::parse(argc, argv);
    if (!args)
        return 1;

    return $EntryPoint(*args);
}
"

    }

    $result = "// This file is generated by New-Parser.ps1; do not edit manually.
$($headers -join [System.Environment]::NewLine)
    
$result"

    $result | Set-Content $OutputPath
}