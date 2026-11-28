# lab.ps1 - everything for the Infineon Live Lab PC in one go.
#
# In a PowerShell terminal on the lab PC (VS Code > Terminal):
#   cd ~\Documents
#   iwr https://raw.githubusercontent.com/NadaAbbas444/inhabitx/main-board-infineon/lab.ps1 -OutFile lab.ps1
#   powershell -ExecutionPolicy Bypass -File lab.ps1            # main with the screen UI
#   powershell -ExecutionPolicy Bypass -File lab.ps1 -NoScreen  # main without screen
#   powershell -ExecutionPolicy Bypass -File lab.ps1 -Check     # only read the board: Wi-Fi, broker, clock
#
# It downloads this branch and Infineon's examples, asks for the broker
# password (Wi-Fi name and password are read from the lab's wifi_connect.bat),
# then builds and programs the board and checks its serial output. Run it again after a failure: finished
# steps are skipped. Passwords end up only in secrets.h on the lab PC.
param([switch]$NoScreen, [switch]$Check)
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

if (-not $Check) {     # -Check skips steps 1-5

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
    Push-Location $app
    make program -j8
    $ok = $LASTEXITCODE -eq 0
    Pop-Location
    if (-not $ok) { throw 'build or programming failed: send the first error above' }
}

# 6. check: read the board's serial output and look for Wi-Fi, broker and clock
Step 'checking the board (close Tera Term first, it would hold the port)'
$dev = Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match 'KitProg3 USB-UART \((COM\d+)\)' } | Select-Object -First 1
if (-not $dev) { throw 'no KitProg3 USB-UART port found' }
$com = [regex]::Match($dev.Name, 'COM\d+').Value
$port = New-Object System.IO.Ports.SerialPort $com, 115200
$port.ReadTimeout = 500
$port.Open()
Write-Host "reading $com. Now press RESET on the lab page (Input Panel > Board Controls)." -ForegroundColor Yellow
$got = @{ wifi = $false; broker = $false; clock = $false }
$hints = @()
$until = (Get-Date).AddSeconds(150)
while ((Get-Date) -lt $until -and -not ($got.wifi -and $got.broker -and $got.clock)) {
    try { $line = $port.ReadLine().TrimEnd() } catch [System.TimeoutException] { continue }
    Write-Host "  board: $line"
    if ($line -match 'Successfully connected to Wi-Fi') { $got.wifi = $true }
    if ($line -match 'MQTT connection successful') { $got.broker = $true }
    if ($line -match 'main: clock set') { $got.clock = $true }
    if ($line -match 'Wi-Fi Connection failed') { $hints += 'Wi-Fi failed: check the Wi-Fi name/password in ixmain\proj_cm33_ns\secrets.h (capital letters count)' }
    if ($line -match 'sntp: no answer') { $hints += 'no internet time (NTP blocked?): give it once with a node message, e.g. on the laptop: .venv/bin/python tools/fake_nodes.py --room Z.1.12' }
    if ($line -match 'MQTT connect failed|Connection refused|not authorized') { $hints += 'broker refused: check the broker password in secrets.h' }
}
$port.Close()
Write-Host ''
foreach ($k in 'wifi', 'broker', 'clock') {
    if ($got[$k]) { Write-Host "  OK      $k" -ForegroundColor Green } else { Write-Host "  MISSING $k" -ForegroundColor Red }
}
$hints | Select-Object -Unique | ForEach-Object { Write-Host "  hint: $_" -ForegroundColor Yellow }
if ($got.wifi -and $got.broker -and $got.clock) {
    Step 'all good: book a room on https://nadaabbas444.github.io/inhabitx/ and watch it here or on the screen'
} else {
    Step 'not all there yet: run  powershell -ExecutionPolicy Bypass -File lab.ps1 -Check  to look again'
}
