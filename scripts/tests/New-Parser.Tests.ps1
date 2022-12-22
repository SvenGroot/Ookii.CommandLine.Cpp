#Requires -PSEdition Core
#Requires -Modules Pester

BeforeAll {
    . (Join-Path $PSScriptRoot "common.ps1")

    $inputPath = Join-Path $PSScriptRoot "input"
    $expectedPath = Join-Path $inputPath "expected"
    $outputPath = Join-Path $PSScriptRoot "output"
    $scriptPath = Join-Path (Split-Path -Parent $PSScriptRoot) "New-Parser.ps1"
    $input1 = Join-Path $inputPath "arguments.h"
    $input2 = Join-Path $inputPath "arguments2.h"
    $inputs = $input1,$input2
}

Describe "New-Parser" {
    It "Generates code" {
        $output = Join-Path $outputPath "parser_default.cpp"
        &$scriptPath $inputs -OutputPath $output
        Compare-Files $output "parser_default.cpp"
    }
    It "Generates code with <_> names" -ForEach "PascalCase","CamelCase","SnakeCase","DashCase","Trim" {
        $output = Join-Path $outputPath "parser_$_.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform $_
        Compare-Files $output "parser_$_.cpp"
    }
    It "Generates a main method" {
        $output = Join-Path $outputPath "parser_main.cpp"
        &$scriptPath $input1 -OutputPath $output -NameTransform PascalCase -EntryPoint "my_entry"
        Compare-Files $output "parser_main.cpp"
    }
    It "Can use wide characters" {
        $output = Join-Path $outputPath "parser_wide.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -WideChar
        Compare-Files $output "parser_wide.cpp"
    }
    It "Generates a main method with wide characters" {
        $output = Join-Path $outputPath "parser_wmain.cpp"
        &$scriptPath $input1 -OutputPath $output -NameTransform PascalCase -EntryPoint "my_entry" -WideChar
        Compare-Files $output "parser_wmain.cpp"
    }
    It "Generates long short mode" {
        $output = Join-Path $outputPath "parser_long_short.cpp"
        &$scriptPath (Join-Path $inputPath "long_short.h") -OutputPath $output
        Compare-Files $output "parser_long_short.cpp"
    }
    It "Generates long short mode with wide characters" {
        $output = Join-Path $outputPath "parser_long_short_wide.cpp"
        &$scriptPath (Join-Path $inputPath "long_short.h") -OutputPath $output -WideChar
        Compare-Files $output "parser_long_short_wide.cpp"
    }
    It "Generates action arguments" {
        $output = Join-Path $outputPath "parser_action_arguments.cpp"
        &$scriptPath (Join-Path $inputPath "action_arguments.h") -OutputPath $output
        Compare-Files $output "parser_action_arguments.cpp"
    }
    It "Can configure automatic arguments" {
        $output = Join-Path $outputPath "parser_auto_arguments.cpp"
        &$scriptPath (Join-Path $inputPath "auto_arguments.h") -OutputPath $output
        Compare-Files $output "parser_auto_arguments.cpp"
    }
    It "Can use additional headers" {
        $output = Join-Path $outputPath "parser_headers.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -AdditionalHeaders "foo.h","bar.h"
        Compare-Files $output "parser_headers.cpp"
    }
    It "Can use LiteralPath" {
        $output = Join-Path $outputPath "parser_default.cpp"
        &$scriptPath -LiteralPath $inputs -OutputPath $output
        Compare-Files $output "parser_default.cpp"
    }
    It "Can use pipeline input" {
        $output = Join-Path $outputPath "parser_default.cpp"
        Get-Item $inputs | &$scriptPath -OutputPath $output
        Compare-Files $output "parser_default.cpp"
    }
}