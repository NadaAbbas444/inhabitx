# lab.ps1 - everything for the Infineon Live Lab PC in one go.
#
# In a PowerShell terminal on the lab PC (VS Code > Terminal):
#   cd ~\Documents
#   iwr https://raw.githubusercontent.com/NadaAbbas444/inhabitx/main-board-infineon/lab.ps1 -OutFile lab.ps1
#   powershell -ExecutionPolicy Bypass -File lab.ps1            # main with the screen UI
#   powershell -ExecutionPolicy Bypass -File lab.ps1 -NoScreen  # main without screen
#
# It downloads this branch and Infineon's examples, asks for the broker
# password (Wi-Fi name and password are read from the lab's wifi_connect.bat),
# then builds and programs the board. Run it again after a failure: finished
# steps are skipped. Passwords end up only in secrets.h on the lab PC.
param([switch]$NoScreen)
$ErrorActionPreference = 'Stop'
Set-Location (Join-Path $HOME 'Documents')

$tools = Join-Path $HOME 'ModusToolbox\tools_3.9'
$creator = Join-Path $tools 'project-creator\project-creator-cli.exe'
$board = 'KIT_PSE84_EVAL_EPC2'

function Step($text) { Write-Host "`n=== $text" -ForegroundColor Cyan }

function New-Project($name, $example) {
    if (Test-Path "$name\Makefile") { Write-Host "$name already there"; return }
    Step "downloading Infineon's $example (a few minutes)"
    & $creator --board-id $board --app-id $example --user-app-name $name --target-dir .
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path "$name\Makefile")) { throw "creating $name failed (run lab.ps1 again)" }
}

# 1. our code
if (-not (Test-Path 'ix\with-screen')) {
    Step 'downloading our code'
    Invoke-WebRequest 'https://github.com/NadaAbbas444/inhabitx/archive/refs/heads/main-board-infineon.zip' -OutFile ix.zip
    Expand-Archive ix.zip -DestinationPath . -Force
    if (Test-Path ix) { Remove-Item ix -Recurse -Force }
    Rename-Item inhabitx-main-board-infineon ix
    Remove-Item ix.zip
}

# 2. Infineon's examples, one after the other (they share mtb_shared)
New-Project 'ixmain' 'mtb-example-psoc-edge-wifi-mqtt-client'
if (-not $NoScreen) { New-Project 'ixscreen' 'mtb-example-psoc-edge-gfx-lvgl-demo' }

# 3. secrets.h: Wi-Fi from the lab's wifi_connect.bat, broker password typed in
$m33 = 'ixmain\proj_cm33_ns'
$sec = "$m33\secrets.h"
if (-not (Test-Path $sec) -and (Test-Path 'secrets.h')) { Copy-Item 'secrets.h' $sec }   # one prepared in Documents
if (-not (Test-Path $sec)) {
    Step 'passwords'
    $ssid = ''; $wpass = ''
    $bat = Get-ChildItem -Path $HOME -Filter wifi_connect.bat -Recurse -Depth 4 -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($bat) {
        foreach ($line in Get-Content $bat.FullName) {
            if ($line -match '^\s*set\s+SSID=(.+)$') { $ssid = $Matches[1].Trim() }
            if ($line -match '^\s*set\s+PASSWORD=(.+)$') { $wpass = $Matches[1].Trim() }
        }
    }
    if ($ssid) { Write-Host "Wi-Fi from $($bat.FullName): $ssid" }
    else {
        $ssid = Read-Host 'Wi-Fi name'
        $wpass = Read-Host 'Wi-Fi password'
    }
    $mpass = Read-Host 'broker password (HiveMQ login "devices")'
    $text = "#ifndef SECRETS_H_`r`n#define SECRETS_H_`r`n" +
            "#define SECRET_WIFI_SSID     `"$ssid`"`r`n" +
            "#define SECRET_WIFI_PASSWORD `"$wpass`"`r`n" +
            "#define SECRET_MQTT_PASSWORD `"$mpass`"`r`n" +
            "#endif`r`n"
    [IO.File]::WriteAllText((Join-Path $PWD $sec), $text)
}

# 4. our files into Infineon's projects
if ($NoScreen) {
    Step 'no-screen files'
    Copy-Item 'ix\no-screen\*' $m33 -Exclude 'secrets.h.example' -Force
    Copy-Item 'ix\common\*' $m33 -Exclude 'test_main_app.c' -Force
    $app = 'ixmain'
} else {
    Step 'with-screen files'
    & (Join-Path $PWD 'ix\with-screen\setup_lab.ps1')
    $app = 'ixscreen'
}

# 5. build + program
Step "building and programming $app (first build: about 5 minutes)"
$env:Path = (Join-Path $tools 'modus-shell\bin') + ';' + $env:Path
$env:CY_TOOLS_PATHS = $tools -replace '\\', '/'
Set-Location $app
make program -j8
if ($LASTEXITCODE -ne 0) { throw 'build or programming failed: send the first error above' }
Step 'done: open Tera Term > Serial > KitProg3 USB-UART, Setup > Serial port > 115200, then press RESET in the lab page'
