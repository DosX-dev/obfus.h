param([Parameter(Mandatory = $true)][string]$Archive, [Parameter(Mandatory = $true)][string]$Destination)
$ErrorActionPreference = 'Stop'
Start-Sleep -Seconds 1
Expand-Archive -LiteralPath $Archive -DestinationPath $Destination