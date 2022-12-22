#Requires -PSEdition Core
#Requires -Modules Pester

BeforeAll {
    . (Join-Path $PSScriptRoot "common.ps1")

    $inputPath = Join-Path $PSScriptRoot "input"
    $outputPath = Join-Path $PSScriptRoot "output"
    $scriptPath = Join-Path (Split-Path -Parent $PSScriptRoot) "New-Parser.ps1"
    $input1 = Join-Path $inputPath "arguments.h"
    $input2 = Join-Path $inputPath "arguments2.h"
    $inputs = $input1,$input2
}

Describe "New-Parser" {
    It "Generates code" {
        $output = Join-Path $outputPath "actual.cpp"
        &$scriptPath $inputs -OutputPath $output
        Compare-Files $output "expected.cpp"
    }
    It "Generates code with <_> names" -ForEach "PascalCase","CamelCase","SnakeCase","DashCase","Trim" {
        $output = Join-Path $outputPath "actual_$_.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform $_
        Compare-Files $output "expected_$_.cpp"
    }
    It "Generates a main method" {
        $output = Join-Path $outputPath "actual_main.cpp"
        &$scriptPath $input1 -OutputPath $output -NameTransform PascalCase -EntryPoint "my_entry"
        Compare-Files $output "expected_main.cpp"
    }
    It "Can use wide characters" {
        $output = Join-Path $outputPath "actual_wide.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -WideChar
        Compare-Files $output "expected_wide.cpp"
    }
    It "Generates a main method with wide characters" {
        $output = Join-Path $outputPath "actual_wmain.cpp"
        &$scriptPath $input1 -OutputPath $output -NameTransform PascalCase -EntryPoint "my_entry" -WideChar
        Compare-Files $output "expected_wmain.cpp"
    }
    It "Generates long short mode" {
        $output = Join-Path $outputPath "actual_long_short.cpp"
        &$scriptPath (Join-Path $inputPath "long_short.h") -OutputPath $output
        Compare-Files $output "expected_long_short.cpp"
    }
    It "Generates long short mode with wide characters" {
        $output = Join-Path $outputPath "actual_long_short_wide.cpp"
        &$scriptPath (Join-Path $inputPath "long_short.h") -OutputPath $output -WideChar
        Compare-Files $output "expected_long_short_wide.cpp"
    }
    It "Generates action arguments" {
        $output = Join-Path $outputPath "actual_action_arguments.cpp"
        &$scriptPath (Join-Path $inputPath "action_arguments.h") -OutputPath $output
        Compare-Files $output "expected_action_arguments.cpp"
    }
    It "Can configure automatic arguments" {
        $output = Join-Path $outputPath "actual_auto_arguments.cpp"
        &$scriptPath (Join-Path $inputPath "auto_arguments.h") -OutputPath $output
        Compare-Files $output "expected_auto_arguments.cpp"
    }
    It "Can use additional headers" {
        $output = Join-Path $outputPath "actual_headers.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -AdditionalHeaders "foo.h","bar.h"
        Compare-Files $output "expected_headers.cpp"
    }
    It "Can use LiteralPath" {
        $output = Join-Path $outputPath "actual.cpp"
        &$scriptPath -LiteralPath $inputs -OutputPath $output
        Compare-Files $output "expected.cpp"
    }
    It "Can use pipeline input" {
        $output = Join-Path $outputPath "actual.cpp"
        Get-Item $inputs | &$scriptPath -OutputPath $output
        Compare-Files $output "expected.cpp"
    }
}