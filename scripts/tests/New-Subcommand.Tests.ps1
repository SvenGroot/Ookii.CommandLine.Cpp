#Requires -PSEdition Core
#Requires -Modules Pester

BeforeAll {
    . (Join-Path $PSScriptRoot "common.ps1")

    $inputPath = Join-Path $PSScriptRoot "input"
    $expectedPath = Join-Path $inputPath "expected"
    $outputPath = Join-Path $PSScriptRoot "output"
    $scriptPath = Join-Path (Split-Path -Parent $PSScriptRoot) "New-Subcommand.ps1"
    $input1 = Join-Path $inputPath "subcommand.h"
    $input2 = Join-Path $inputPath "subcommand2.h"
    $inputs = $input1,$input2
}

Describe "New-Parser" {
    It "Generates code" {
        $output = Join-Path $outputPath "sc_default.cpp"
        &$scriptPath $inputs -OutputPath $output
        Compare-Files $output "sc_default.cpp"
    }
    It "Generates code with <_> names" -ForEach "PascalCase","CamelCase","SnakeCase","DashCase","Trim" {
        $output = Join-Path $outputPath "sc_$_.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform $_
        Compare-Files $output "sc_$_.cpp"
    }
    It "Generates a main method" {
        $output = Join-Path $outputPath "sc_main.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -GenerateMain
        Compare-Files $output "sc_main.cpp"
    }
    It "Can use wide characters" {
        $output = Join-Path $outputPath "sc_wide.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -WideChar
        Compare-Files $output "sc_wide.cpp"
    }
    It "Generates a main method with wide characters" {
        $output = Join-Path $outputPath "sc_wmain.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -GenerateMain -WideChar
        Compare-Files $output "sc_wmain.cpp"
    }
    It "Generates global parser config" {
        $output = Join-Path $outputPath "sc_global.cpp"
        &$scriptPath (Join-Path $inputPath "subcommand_global.h") -OutputPath $output -NameTransform PascalCase
        Compare-Files $output "sc_global.cpp"
    }
    It "Can use additional headers" {
        $output = Join-Path $outputPath "sc_headers.cpp"
        &$scriptPath $inputs -OutputPath $output -NameTransform PascalCase -AdditionalHeaders "foo.h","bar.h"
        Compare-Files $output "sc_headers.cpp"
    }
    It "Can use LiteralPath" {
        $output = Join-Path $outputPath "sc_default.cpp"
        &$scriptPath -LiteralPath $inputs -OutputPath $output
        Compare-Files $output "sc_default.cpp"
    }
    It "Can use pipeline input" {
        $output = Join-Path $outputPath "sc_default.cpp"
        Get-Item $inputs | &$scriptPath -OutputPath $output
        Compare-Files $output "sc_default.cpp"
    }
}