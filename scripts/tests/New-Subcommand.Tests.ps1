#Requires -PSEdition Core
#Requires -Modules Pester

BeforeAll {
    . (Join-Path $PSScriptRoot "common.ps1")

    $inputPath = Join-Path $PSScriptRoot "input"
    $outputPath = Join-Path $PSScriptRoot "output"
    $scriptPath = Join-Path (Split-Path -Parent $PSScriptRoot) "New-Subcommand.ps1"
    $input1 = Join-Path $inputPath "subcommand.h"
    $input2 = Join-Path $inputPath "subcommand2.h"
    $inputs = $input1,$input2
}

Describe "New-Parser" {
    It "Generates code" {
        $output = Join-Path $outputPath "sc_actual.cpp"
        &$scriptPath $inputs -OutputPath $output
        Compare-Files $output "sc_expected.cpp"
    }
    It "Generates code with <_> names" -ForEach "PascalCase","CamelCase","SnakeCase","DashCase","Trim" {
        $output = Join-Path $outputPath "sc_actual_$_.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform $_
        Compare-Files $output "sc_expected_$_.cpp"
    }
    It "Generates a main method" {
        $output = Join-Path $outputPath "sc_actual_main.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -GenerateMain
        Compare-Files $output "sc_expected_main.cpp"
    }
    It "Can use wide characters" {
        $output = Join-Path $outputPath "sc_actual_wide.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -WideChar
        Compare-Files $output "sc_expected_wide.cpp"
    }
    It "Generates a main method with wide characters" {
        $output = Join-Path $outputPath "sc_actual_wmain.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -GenerateMain -WideChar
        Compare-Files $output "sc_expected_wmain.cpp"
    }
    It "Can use additional headers" {
        $output = Join-Path $outputPath "sc_actual_headers.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -AdditionalHeaders "foo.h","bar.h"
        Compare-Files $output "sc_expected_headers.cpp"
    }
    It "Can use LiteralPath" {
        $output = Join-Path $outputPath "sc_actual.cpp"
        &$scriptPath -LiteralPath $inputs -OutputPath $output
        Compare-Files $output "sc_expected.cpp"
    }
    It "Can use pipeline input" {
        $output = Join-Path $outputPath "sc_actual.cpp"
        Get-Item $inputs | &$scriptPath -OutputPath $output
        Compare-Files $output "sc_expected.cpp"
    }
}