#Script to encapsulate test running just so the correct exit code can be returned.
param(
    [Parameter(Mandatory=$true)][string]$File
)

&$File
exit $LASTEXITCODE