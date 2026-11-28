# Builds with the 4 MB app limit explicitly, independent of Arduino IDE's saved menu selection.
[CmdletBinding()]
param(
    [string]$ArduinoCli = 'C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe',
    [string]$ConfigFile = '',
    [switch]$CheckOnly
)
$ErrorActionPreference = 'Stop'
if (-not (Test-Path -LiteralPath $ArduinoCli -PathType Leaf)) {
    $command = Get-Command arduino-cli -ErrorAction SilentlyContinue
    if (-not $command) { throw 'arduino-cli.exe not found. Pass -ArduinoCli with its path.' }
    $ArduinoCli = $command.Source
}
$csvPath = Join-Path $PSScriptRoot 'partitions.csv'
if (-not (Test-Path -LiteralPath $csvPath)) { throw 'Keep partitions.csv beside this script.' }
$rows = @(Get-Content -LiteralPath $csvPath | Where-Object { $_.Trim() -and -not $_.TrimStart().StartsWith('#') } | ConvertFrom-Csv -Header Name,Type,SubType,Offset,Size,Flags)
$app = $rows | Where-Object { $_.Name.Trim() -eq 'app0' }
$model = $rows | Where-Object { $_.Name.Trim() -eq 'model' }
if (-not $app -or $app.Size.Trim() -ne '0x400000' -or -not $model -or $model.Offset.Trim() -ne '0xC10000') {
    throw 'Unexpected partition layout: need 4 MB app0 and model at 0xC10000.'
}
# Stock ESP SR selection supplies model-flashing flags; local CSV supplies the 4 MB layout.
# Override its 3 MB IDE maximum here. No boards.local.txt installation is required.
$fqbn = 'esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,USBMode=hwcdc,CDCOnBoot=cdc,PartitionScheme=esp_sr_16'
$buildPath = Join-Path (Split-Path -Parent $PSScriptRoot) 'build-voice-local'
$compileArgs = @('compile', '--fqbn', $fqbn, '--build-property', 'upload.maximum_size=4194304', '--build-path', $buildPath)
if (-not $ConfigFile) {
    $workspaceConfig = Join-Path $PSScriptRoot '..\..\arduino-cli.yaml'
    if (Test-Path -LiteralPath $workspaceConfig) { $ConfigFile = (Resolve-Path -LiteralPath $workspaceConfig).Path }
}
if ($ConfigFile) { $compileArgs += @('--config-file', $ConfigFile) }
$propertyLines = @(& $ArduinoCli @compileArgs '--show-properties=expanded' $PSScriptRoot)
if ($LASTEXITCODE -ne 0) { throw 'Could not resolve Arduino build settings.' }
$properties = @{}
foreach ($line in $propertyLines) {
    $parts = ([string]$line).Split('=', 2)
    if ($parts.Count -eq 2) { $properties[$parts[0]] = $parts[1] }
}
if ($properties['upload.maximum_size'] -ne '4194304' -or
    $properties['build.flash_size'] -ne '16MB' -or
    $properties['upload.extra_flags'] -notmatch '^0xC10000\s') {
    throw 'Build properties do not match 4 MB application / 16 MB flash / speech-model upload at 0xC10000.'
}
Write-Host 'Verified: 4 MB app maximum, 16 MB flash, OPI PSRAM, speech models at 0xC10000.'
if ($CheckOnly) { return }
& $ArduinoCli @compileArgs $PSScriptRoot
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed; see the compiler output above.' }
$builtBin = Join-Path $buildPath 'ClassroomOccupancy.ino.bin'
if (-not (Test-Path -LiteralPath $builtBin) -or (Get-Item -LiteralPath $builtBin).Length -gt 4194304) {
    throw 'Expected application binary missing or larger than the app slot.'
}
Write-Host "Build passed. Output: $buildPath"
Write-Host 'No device was flashed. For IDE upload, choose the installed InhabitX Voice 16M option first.'
