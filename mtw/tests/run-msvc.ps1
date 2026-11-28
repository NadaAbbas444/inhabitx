$ErrorActionPreference = 'Stop'
# Run from a Visual Studio Developer PowerShell with cl.exe on PATH.
$testDir = $PSScriptRoot
$buildDir = Join-Path $testDir 'test-build'
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Push-Location $buildDir
try {
    & cl /nologo /std:c11 /O2 (Join-Path $testDir 'occupancy_test.c') /Feoccupancy_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Occupancy test compilation failed' }
    & .\occupancy_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Occupancy test failed' }
    & cl /nologo /std:c11 /O2 /Drestrict=__restrict (Join-Path $testDir 'model_test.c') (Join-Path $testDir '../Inhabix/proj_cm55/model/model.c') /Femodel_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Model test compilation failed' }
    & .\model_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Model test failed' }
    & cl /nologo /std:c11 /O2 ('/I' + (Join-Path $testDir 'mock')) (Join-Path $testDir 'pipeline_test.c') /Fepipeline_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Pipeline test compilation failed' }
    & .\pipeline_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Pipeline test failed' }
} finally {
    Pop-Location
}
