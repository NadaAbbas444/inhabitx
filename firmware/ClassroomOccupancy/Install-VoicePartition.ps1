# Registers a separate board menu option; preserves the stock ESP SR 16M option.
# Close Arduino IDE first. Run this file from the downloaded sketch folder.
[CmdletBinding()]
param(
    [string]$PlatformPath = (Join-Path $env:LOCALAPPDATA 'Arduino15\packages\esp32\hardware\esp32\3.3.12')
)
$ErrorActionPreference = 'Stop'
$platformRoot = (Resolve-Path -LiteralPath $PlatformPath).Path
$boardsPath = Join-Path $platformRoot 'boards.txt'
if (-not (Test-Path -LiteralPath $boardsPath -PathType Leaf)) {
    throw "Arduino-ESP32 boards.txt not found at $boardsPath. Install esp32 3.3.12 or pass -PlatformPath."
}
$sourceCsv = Join-Path $PSScriptRoot 'partitions.csv'
if (-not (Test-Path -LiteralPath $sourceCsv -PathType Leaf)) {
    throw 'Keep partitions.csv beside this script.'
}
$targetCsv = Join-Path $platformRoot 'tools\partitions\inhabitx_sr_16.csv'
$localBoards = Join-Path $platformRoot 'boards.local.txt'
$prefix = 'esp32s3.menu.PartitionScheme.inhabitx_sr_16.'
$menuKey = 'esp32s3.menu.PartitionScheme.inhabitx_sr_16='
$existing = @()
if (Test-Path -LiteralPath $localBoards) {
    Copy-Item -LiteralPath $localBoards -Destination ($localBoards + '.inhabitx-backup-' + (Get-Date -Format 'yyyyMMdd-HHmmss')) -ErrorAction Stop
    $existing = @(Get-Content -LiteralPath $localBoards | Where-Object {
        -not $_.StartsWith($prefix) -and -not $_.StartsWith($menuKey)
    })
}
$entries = @(
    'esp32s3.menu.PartitionScheme.inhabitx_sr_16=InhabitX Voice 16M (4MB APP/4MB SPIFFS/3.9MB MODEL)'
    'esp32s3.menu.PartitionScheme.inhabitx_sr_16.upload.maximum_size=4194304'
    'esp32s3.menu.PartitionScheme.inhabitx_sr_16.upload.extra_flags=0xC10000 {build.path}/srmodels.bin'
    'esp32s3.menu.PartitionScheme.inhabitx_sr_16.build.partitions=inhabitx_sr_16'
)
Copy-Item -LiteralPath $sourceCsv -Destination $targetCsv -Force
[IO.File]::WriteAllText($localBoards, (($existing + $entries) -join [Environment]::NewLine) + [Environment]::NewLine, [Text.UTF8Encoding]::new($false))
Write-Host 'Installed: InhabitX Voice 16M (4MB APP/4MB SPIFFS/3.9MB MODEL)'
Write-Host 'Restart Arduino IDE, select ESP32S3 Dev Module, then select that Partition Scheme.'
Write-Host 'Keep Flash Size=16MB and PSRAM=OPI PSRAM. Verify and Upload again.'
Write-Host 'After an esp32 core update, rerun this script with -PlatformPath pointing to the new version.'
