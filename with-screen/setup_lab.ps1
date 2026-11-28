# setup_lab.ps1 - builds the with-screen project from the two Infineon examples.
# Run in the folder that holds ix\ (this branch), ixmain\ (Wi-Fi MQTT Client,
# with secrets.h filled in) and ixscreen\ (Graphics LVGL demo):
#   powershell -ExecutionPolicy Bypass -File ix\with-screen\setup_lab.ps1
# Does steps 2-7 of the README's with-screen section. Passwords are not in here:
# secrets.h is taken from ixmain\proj_cm33_ns.
$ErrorActionPreference = 'Stop'
$ix = Join-Path $PWD 'ix'
$w  = Join-Path $PWD 'ixmain\proj_cm33_ns'
$s  = Join-Path $PWD 'ixscreen'
$m33 = Join-Path $s 'proj_cm33_ns'
$m55 = Join-Path $s 'proj_cm55\source'

function Edit-File($path, $from, $to) {
    $t = [IO.File]::ReadAllText($path)
    if (-not $t.Contains($from)) { throw "not found in ${path}: $from" }
    $i = $t.IndexOf($from)
    $t = $t.Substring(0, $i) + $to + $t.Substring($i + $from.Length)
    [IO.File]::WriteAllText($path, $t)
    Write-Host "edited $path"
}

# 2. Wi-Fi core: Infineon's Wi-Fi project, then our relay on top
Copy-Item "$w\*" $m33 -Recurse -Force
Remove-Item "$m33\ix_model.*", "$m33\ix_msg.*", "$m33\main_app.*", "$m33\test_main_app.c" -ErrorAction SilentlyContinue
Copy-Item "$ix\with-screen\cm33\*", "$ix\with-screen\shared\*", "$ix\common\main_rooms.h" $m33 -Force
if (-not (Test-Path "$m33\secrets.h")) { throw "secrets.h missing in $m33" }

# 3. Wi-Fi core main.c: set up the message pipes before starting the screen core
$main33 = "$m33\main.c"
if (-not ([IO.File]::ReadAllText($main33)).Contains('ix_ipc_cm33_init')) {
    Edit-File $main33 'Cy_SysEnableCM55(' "ix_ipc_cm33_init();`r`n    Cy_SysEnableCM55("
    $t = [IO.File]::ReadAllText($main33)
    [IO.File]::WriteAllText($main33, "#include `"ix_ipc.h`"`r`n" + $t)
}

# 4. Screen core: our screen, UI and logic
Copy-Item "$ix\with-screen\cm55\*", "$ix\with-screen\shared\*", "$ix\with-screen\ui\*" $m55 -Force
Copy-Item "$ix\common\*" $m55 -Exclude 'test_main_app.c' -Force

# 5. Screen core main.c: our UI instead of the music demo
$main55 = "$m55\main.c"
if (-not ([IO.File]::ReadAllText($main55)).Contains('inhabitx_screen_start')) {
    Edit-File $main55 'lv_demo_music();' 'inhabitx_screen_start();'
    Edit-File $main55 'time_till_next = lv_timer_handler();' 'time_till_next = inhabitx_screen_loop();'
    Edit-File $main55 '#include "lvgl.h"' "#include `"lvgl.h`"`r`n#include `"inhabitx_screen.h`""
}

# 6. lv_conf.h: our fonts
$conf = Join-Path $s 'proj_cm55\include\lv_conf.h'
$t = [IO.File]::ReadAllText($conf)
$t = $t -replace '#define LV_USE_TINY_TTF\s+0', '#define LV_USE_TINY_TTF 1'
$t = $t -replace '#define LV_FONT_MONTSERRAT_32\s+0', '#define LV_FONT_MONTSERRAT_32 1'
$t = $t -replace '#define LV_FONT_MONTSERRAT_16\s+0', '#define LV_FONT_MONTSERRAT_16 1'
$t = $t -replace '#define LV_FONT_DEFAULT\s+&lv_font_montserrat_14', '#define LV_FONT_DEFAULT &lv_font_montserrat_16'
[IO.File]::WriteAllText($conf, $t)
Write-Host "edited $conf"

# 7. Makefile: screen size of the 4.3-inch display (CONFIG_DISPLAY default)
$mk = Join-Path $s 'proj_cm55\Makefile'
if (-not ([IO.File]::ReadAllText($mk)).Contains('UI_HOR_RES')) {
    Edit-File $mk 'DEFINES+=CY_RETARGET_IO_CONVERT_LF_TO_CRLF' "DEFINES+=UI_HOR_RES=832 UI_VER_RES=480`r`nDEFINES+=CY_RETARGET_IO_CONVERT_LF_TO_CRLF"
}

Write-Host 'done: now run  make program -j8  in ixscreen'
