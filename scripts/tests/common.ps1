function Get-ContentWithLineNumbers([string]$Path) {
    Get-Content $Path | ForEach-Object `
        -Begin { $i = 1 } `
        -Process { 
            $_ | Add-Member -Type NoteProperty -Name Line -Value $i
            $i += 1
            $_
        }
}

function Compare-Files([string]$Actual, [string]$Expected, [switch]$Negate, [string]$Because)
{
    if ($Negate) {
        throw "Negate not supported."
    }

    $expectedContent = Get-ContentWithLineNumbers (Join-Path $inputPath $Expected)
    $actualContent = Get-ContentWithLineNumbers $Actual
    [bool]$succeeded = $true
    $differences = Compare-Object $expectedContent $actualContent -CaseSensitive -PassThru
    if ($differences) {
        $succeeded = $false
        $differences = ($differences | ForEach-Object {
            "$($_.Line). [$($_.SideIndicator)] $_"
        }) -join [Environment]::NewLine

        $because = "the file '$Actual' differs from the file '$Expected':`n$differences"
    }
    
    $succeeded | Should -BeTrue -Because $because
}
