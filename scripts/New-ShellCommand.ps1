#Requires -PSEdition Core

<#
    .SYNOPSIS

        Generates command line argument parsers for shell commands from annotated C++ source code.

    .DESCRIPTION

        This script takes one or more C++ header files, and parses them for specially annotated
        shell_command classes.

        For each of those classes, it will generate the constructor which defines the arguments
        for that command. It will also create a function, ookii::register_shell_commands, that
        registers all the found commands.

        Optionally, it can also generate a main() method for your application, which parses the
        arguments and runs the specified command, printing error messages and usage help as
        appropriate.

        For information on how to annotate your shell commands to generate arguments, refer to
        the Ookii.CommandLine.Cpp documentation.

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
    # Generates a main method that will register the shell commands, and try to execute one based
    # on the command line parameters. When using this option, there is no need to provide any 
    # entry point manually.
    [Parameter()][switch]$GenerateMain,
    # Supplies additional header files that should be included in the generated result file before
    # any other header files. Use this to include, for example, a precompiled header file. Note
    # that the provided values are used as is, so the path must be relative to the generated output
    # or the file must be on the include path. Usually, it's easier to include required headers
    # in the header(s) passed to -Path or -LiteralPath.
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
        "#include <ookii/shell_command.h>",
        "#include <ookii/command_line_generated.h>"

    if ($WideChar) {
        $stringPrefix = "L"
        $charType = "wchar_t"
        $mainName = "wmain"
    } else {
        $stringPrefix = ""
        $charType = "char"
        $mainName = "main"
    }

    $commands = @()
}
process {
    if ($Path) {
        $inputs = Get-Item -Path $Path
    } else {
        $inputs = Get-Item -Path $LiteralPath
    }

    foreach ($file in $inputs)
    {
        $contents = $file | Get-Content
        # Get the relative path to the header from the generated output file, and always use / even on Windows.
        $fileName = [System.IO.Path]::GetRelativePath($outputDir, $file.FullName).Replace([System.IO.Path]::DirectorySeparatorChar, [System.IO.Path]::AltDirectorySeparatorChar)
        $headers += "#include `"$fileName`""
        foreach ($info in (Convert-Arguments $contents "shell_command")) {
            $commands += $info
            $result += $info.GenerateShellCommand($stringPrefix, $NameTransform) -join [System.Environment]::NewLine
        }
    }
}
end {
    if ($commands.Length -eq 0) {
        throw "No arguments types found."
    }

    $result += "ookii::basic_shell_command_manager<$charType> ookii::register_shell_commands(std::basic_string<$charType> application_name)
{
    basic_shell_command_manager<$charType> manager{application_name};
    manager
$(($commands | ForEach-Object { if ($_.Register) { $_.GenerateRegistration($stringPrefix) } }) -join [System.Environment]::NewLine);

    return manager;
}

"

    if ($GenerateMain) {
        $conversion = if ($charType -eq "wchar_t") {
            "wstring"
        } else {
            "string"
        }

        $result += "int $mainName(int argc, $charType *argv[])
{
    std::basic_string<$charType> name;
    if (argc > 0)
        name = std::filesystem::path{argv[0]}.filename().$conversion();
    auto manager = ookii::register_shell_commands(name);
    return manager.run_command(argc, argv);
}
"

    }

    $result = "// This file is generated by New-ShellCommand.ps1; do not edit manually.
$($headers -join [System.Environment]::NewLine)
    
$result"

    $result | Set-Content $OutputPath
}