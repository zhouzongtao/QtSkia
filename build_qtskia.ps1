# QtSkia Build Script for VS2022
Write-Host "========================================"  -ForegroundColor Cyan
Write-Host "QtSkia Build Script - VS2022 Static" -ForegroundColor Cyan
Write-Host "========================================"  -ForegroundColor Cyan
Write-Host ""

# Setup VS environment
Write-Host "Setting up Visual Studio 2022 Enterprise environment..." -ForegroundColor Yellow
$vsDevCmd = "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
if (Test-Path $vsDevCmd) {
    cmd /c "`"$vsDevCmd`" && set > vs_env.txt"
    Get-Content vs_env.txt | ForEach-Object {
        if ($_ -match '^(.+?)=(.*)$') {
            [Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
        }
    }
    Remove-Item vs_env.txt -ErrorAction SilentlyContinue
    Write-Host "VS environment loaded" -ForegroundColor Green
} else {
    Write-Host "ERROR: VS2022 vcvars64.bat not found at: $vsDevCmd" -ForegroundColor Red
    exit 1
}

# Add Qt to PATH
$env:PATH = "E:\Qt\6.10.1\msvc2022_64\bin;" + $env:PATH

# Set prebuilt Skia paths
$env:SKIA_PREBUILT_PATH = "E:\Repository\skia"
$env:SKIA_PREBUILT_OUT = "E:\Repository\skia\out\msvc.x64.release"

Write-Host "Using Qt from: E:\Qt\6.10.1\msvc2022_64" -ForegroundColor Cyan
Write-Host "Using prebuilt Skia from: E:\Repository\skia" -ForegroundColor Cyan
Write-Host "Skia library path: E:\Repository\skia\out\msvc.x64.release" -ForegroundColor Cyan
Write-Host "Static build" -ForegroundColor Cyan
Write-Host ""

# Change to QtSkia directory
Set-Location "E:\Repository\QtSkia"

# Clean old files
Write-Host "Step 0: Cleaning old build files..." -ForegroundColor Yellow
Remove-Item -Path "Makefile*" -Force -ErrorAction SilentlyContinue
Get-ChildItem -Recurse -Directory "debug", "release" | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
Write-Host "Done" -ForegroundColor Green
Write-Host ""

# Run qmake
Write-Host "Step 1: Running qmake..." -ForegroundColor Yellow
$env:SKIA_PREBUILT_PATH = "E:\Repository\skia"
$env:SKIA_PREBUILT_OUT = "E:\Repository\skia\out\msvc.x64.release"
& "E:\Qt\6.10.1\msvc2022_64\bin\qmake.exe" QtSkia.pro -spec win32-msvc "CONFIG+=release"
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: qmake failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
Write-Host "Done" -ForegroundColor Green
Write-Host ""

# Run nmake
Write-Host "Step 2: Building with nmake..." -ForegroundColor Yellow
& nmake
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: nmake failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
Write-Host "Done" -ForegroundColor Green
Write-Host ""

Write-Host "========================================"  -ForegroundColor Cyan
Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "========================================"  -ForegroundColor Cyan
Write-Host ""
Write-Host "Output directory: bin\msvc\static\release\" -ForegroundColor Cyan
Write-Host ""
