# lab_node.ps1 - the board becomes node Z.1.09 (microphone -> Wi-Fi -> broker),
# on the Infineon Live Lab PC in one go.
#
# In a PowerShell terminal on the lab PC (VS Code > Terminal), three short lines:
#   cd ~\Documents
#   iwr https://raw.githubusercontent.com/NadaAbbas444/inhabitx/feat-run-node-on-board/lab_node.ps1 -OutFile lab_node.ps1
#   powershell -ExecutionPolicy Bypass -File lab_node.ps1
# Options (add at the end of the last line):
#   -Check    only read the board again (no build)
#   -Update   fetch this branch again and rebuild (keeps secrets.h)
#   -NoMic    fallback without the audio core: Infineon's plain Wi-Fi example
#             with our node files, always "empty" - tests Wi-Fi + broker only
#
# The node is put together here: the audio app (mtw/Inhabix: microphone,
# model and occupancy filter on the CM55) with Infineon's Wi-Fi MQTT Client
# core (proj_cm33_ns) and our node files (board/) instead of its sleeping
# CM33. Wi-Fi name and password come from the lab's wifi_connect.bat, the
# broker password is typed in (hidden). Passwords end up only in secrets.h.
param([switch]$Check, [switch]$Update, [switch]$NoMic)
$ErrorActionPreference = 'Stop'
Set-Location (Join-Path $HOME 'Documents')

$tools  = Join-Path $HOME 'ModusToolbox\tools_3.9'
$branch = 'feat-run-node-on-board'
$ifxSha = '59e794afe9c4379d95cd0d60ce1fb31ced0ff2e5'     # mtb-example-psoc-edge-wifi-mqtt-client 2.3.0
$app    = 'ixnode'
$m33    = "$app\proj_cm33_ns"

function Step($text) { Write-Host "`n=== $text" -ForegroundColor Cyan }
function CString($s) { '"' + ($s -replace '\\', '\\' -replace '"', '\"') + '"' }
function Get-Zip($url, $folder) {
    Invoke-WebRequest $url -OutFile "$folder.zip"
    $tmp = "$folder.tmp"
    if (Test-Path $tmp) { Remove-Item $tmp -Recurse -Force }
    Expand-Archive "$folder.zip" -DestinationPath $tmp -Force
    if (Test-Path $folder) { Remove-Item $folder -Recurse -Force }
    Move-Item (Get-ChildItem $tmp | Select-Object -First 1).FullName $folder
    Remove-Item $tmp, "$folder.zip" -Recurse -Force
}

if (-not $Check) {     # -Check skips steps 1-5

# 1. our code (ixn) and Infineon's Wi-Fi example sources (ifx-wifi)
if ($Update -or -not (Test-Path 'ixn\board')) {
    Step 'downloading our code'
    Get-Zip "https://github.com/NadaAbbas444/inhabitx/archive/refs/heads/$branch.zip" 'ixn'
}
if (-not (Test-Path 'ifx-wifi\proj_cm33_ns')) {
    Step "downloading Infineon's Wi-Fi MQTT Client example (sources)"
    Get-Zip "https://github.com/Infineon/mtb-example-psoc-edge-wifi-mqtt-client/archive/$ifxSha.zip" 'ifx-wifi'
}

# 2. put the node together (only once, or again with -Update; keeps secrets.h)
$kind = if ($NoMic) { 'nomic' } else { 'mic' }
$built = if (Test-Path "$app\.inhabitx-kind") { Get-Content "$app\.inhabitx-kind" } else { '' }
if ($Update -or $built -ne $kind) {
    Step "putting the node together ($kind)"
    $secret = $null
    if (Test-Path "$m33\secrets.h") { $secret = [IO.File]::ReadAllText((Join-Path $PWD "$m33\secrets.h")) }
    if (Test-Path $app) { Remove-Item $app -Recurse -Force }
    New-Item -ItemType Directory $app | Out-Null
    if ($NoMic) {
        Copy-Item 'ifx-wifi\*' $app -Recurse -Force
    } else {
        Copy-Item 'ixn\mtw\Inhabix\*' $app -Recurse -Force             # audio app: CM55 listens
        Remove-Item $m33 -Recurse -Force                               # its CM33 only slept
        Copy-Item 'ifx-wifi\proj_cm33_ns' $m33 -Recurse -Force         # Infineon's Wi-Fi + MQTT core
    }
    Copy-Item 'ixn\board\*' $m33 -Exclude 'secrets.h.example' -Force # our node: what to send, where
    Copy-Item 'ixn\mtw\Inhabix\shared\node_link.h' $m33 -Force         # the mailbox from the CM55
    if ($NoMic) {
        $mk = Join-Path $PWD "$m33\Makefile"
        $t = [IO.File]::ReadAllText($mk)
        $t = $t.Replace('include $(CY_TOOLS_DIR)/make/start.mk', "DEFINES+=NODE_NO_MIC`r`ninclude `$(CY_TOOLS_DIR)/make/start.mk")
        [IO.File]::WriteAllText($mk, $t)
    }
    if ($secret) { [IO.File]::WriteAllText((Join-Path $PWD "$m33\secrets.h"), $secret) }
    Set-Content "$app\.inhabitx-kind" $kind
}

# 3. secrets.h: Wi-Fi from the lab's wifi_connect.bat, broker password typed in
$sec = "$m33\secrets.h"
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
    $s = Read-Host 'broker password (HiveMQ login "devices")' -AsSecureString
    $mpass = [Runtime.InteropServices.Marshal]::PtrToStringBSTR([Runtime.InteropServices.Marshal]::SecureStringToBSTR($s))
    $text = "#ifndef SECRETS_H_`r`n#define SECRETS_H_`r`n" +
            "#define SECRET_WIFI_SSID     $(CString $ssid)`r`n" +
            "#define SECRET_WIFI_PASSWORD $(CString $wpass)`r`n" +
            "#define SECRET_MQTT_PASSWORD $(CString $mpass)`r`n" +
            "#endif`r`n"
    [IO.File]::WriteAllText((Join-Path $PWD $sec), $text)
}

# 4. libraries (first time: about 10 minutes) + 5. build and program
    $env:Path = (Join-Path $tools 'modus-shell\bin') + ';' + $env:Path
    $env:CY_TOOLS_PATHS = $tools -replace '\\', '/'
    Push-Location $app
    if (-not (Test-Path '.inhabitx-libs')) {
        Step 'downloading the libraries (first time: about 10 minutes)'
        make getlibs
        if ($LASTEXITCODE -ne 0) { Pop-Location; throw 'getlibs failed: run lab_node.ps1 again (downloads resume)' }
        Set-Content '.inhabitx-libs' 'ok'
    }
    Step "building and programming $app (first build: about 5 minutes)"
    make program -j8
    $ok = $LASTEXITCODE -eq 0
    Pop-Location
    if (-not $ok) { throw 'build or programming failed: send the first error above (or try -NoMic to test Wi-Fi + broker alone)' }
}

# 6. check: read the board's serial output (both cores print on it)
Step 'checking the board (close Tera Term first, it would hold the port)'
$dev = Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match 'KitProg3 USB-UART \((COM\d+)\)' } | Select-Object -First 1
if (-not $dev) { throw 'no KitProg3 USB-UART port found' }
$com = [regex]::Match($dev.Name, 'COM\d+').Value
$port = New-Object System.IO.Ports.SerialPort $com, 115200
$port.ReadTimeout = 500
$port.Open()
Write-Host "reading $com. Now press RESET on the lab page (Input Panel > Board Controls)." -ForegroundColor Yellow
$mic = -not (Test-Path "$app\.inhabitx-kind") -or ((Get-Content "$app\.inhabitx-kind") -eq 'mic')
$got = [ordered]@{ wifi = $false; broker = $false }
if ($mic) { $got.audio = $false; $got.room_known = $false }
$got.sending = $false
$script:hints = @()
function Read-Board($seconds, [scriptblock]$done) {
    $until = (Get-Date).AddSeconds($seconds)
    while ((Get-Date) -lt $until -and -not (& $done)) {
        try { $line = $port.ReadLine().TrimEnd() } catch [System.TimeoutException] { continue }
        Write-Host "  board: $line"
        if ($line -match 'Successfully connected to Wi-Fi') { $got.wifi = $true }
        if ($line -match 'MQTT connection successful') { $got.broker = $true }
        if ($line -match 'InhabitX classroom occupancy|^Room: ') { $got.audio = $true }
        if ($line -match '^Room: (OCCUPIED|UNOCCUPIED)|audio says (OCCUPIED|EMPTY)') { $got.room_known = $true }
        if ($line -match 'node: inhabitx/\S+/state -> ') { $got.sending = $true }
        if ($line -match 'Wi-Fi Connection failed') { $script:hints += "Wi-Fi failed: check the Wi-Fi name/password in $m33\secrets.h (capital letters count)" }
        if ($line -match 'MQTT connection failed') { $script:hints += 'broker refused: wrong broker password? delete ' + $sec + ' and run again to retype it' }
        if ($line -match 'no news from the audio core') { $script:hints += 'the audio core is silent: look for its "InhabitX classroom occupancy" line; if missing, try -NoMic to test Wi-Fi alone' }
        if ($line -match 'FATAL|System initialization fail') { $script:hints += 'the audio core failed to start: send the line above' }
    }
}
Read-Board 180 { -not ($got.Values -contains $false) }
Write-Host ''
foreach ($k in $got.Keys) {
    if ($got[$k]) { Write-Host "  OK      $k" -ForegroundColor Green } else { Write-Host "  MISSING $k" -ForegroundColor Red }
}
if ($mic -and -not $got.room_known) { $script:hints += 'room still UNKNOWN: normal for the first ~20 s of silence; talk into the Microphone panel, or run -Check again' }
$script:hints | Select-Object -Unique | ForEach-Object { Write-Host "  hint: $_" -ForegroundColor Yellow }
if ($got.sending) {
    Step 'the node is online: Z.1.09 on the laptop main and https://nadaabbas444.github.io/inhabitx/'
    if ($mic) { Write-Host 'Microphone panel (left): unmute and talk for ~10 s -> OCCUPIED. Then silence: back to empty after ~60-70 s.' -ForegroundColor Yellow }
    Write-Host 'Showing the board for 3 more minutes (Ctrl+C to stop).' -ForegroundColor Yellow
    Read-Board 180 { $false }
} else {
    Step 'not all there yet: run  powershell -ExecutionPolicy Bypass -File lab_node.ps1 -Check  to look again'
}
$port.Close()
