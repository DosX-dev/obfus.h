param([Parameter(Mandatory = $true)][string]$Archive, [Parameter(Mandatory = $true)][string]$Destination)
$ErrorActionPreference = 'Stop'
Expand-Archive -LiteralPath $Archive -DestinationPath $Destination
