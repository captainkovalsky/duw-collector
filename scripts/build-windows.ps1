# PowerShell script to build DUW Collector for Windows

Write-Host "Building DUW Collector for Windows..." -ForegroundColor Green

# Check if cmake is installed
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "Error: cmake is not installed. Please install cmake first." -ForegroundColor Red
    Write-Host "You can download it from: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

# Check if Visual Studio Build Tools are available
if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    Write-Host "Error: Visual Studio Build Tools are not installed or not in PATH." -ForegroundColor Red
    Write-Host "Please install Visual Studio Build Tools or Visual Studio Community." -ForegroundColor Yellow
    exit 1
}

# Check for vcpkg
if (-not (Test-Path "C:\vcpkg")) {
    Write-Host "Warning: vcpkg not found. You may need to install dependencies manually." -ForegroundColor Yellow
    Write-Host "Consider installing vcpkg for easier dependency management." -ForegroundColor Yellow
}

# Create build directory
if (-not (Test-Path "build-windows")) {
    New-Item -ItemType Directory -Name "build-windows" | Out-Null
}
Set-Location "build-windows"

# Configure and build
Write-Host "Configuring project..." -ForegroundColor Blue
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64

Write-Host "Building project..." -ForegroundColor Blue
cmake --build . --config Release

Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "Executable: build-windows\Release\duw-collector.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "To run:" -ForegroundColor Yellow
Write-Host "  cd build-windows\Release && .\duw-collector.exe" -ForegroundColor White
Write-Host ""
Write-Host "Environment variables:" -ForegroundColor Yellow
Write-Host "  `$env:MODE='polling'          # Run in polling mode" -ForegroundColor White
Write-Host "  `$env:DB_PATH='path\to\db'   # Custom database path" -ForegroundColor White
Write-Host "  `$env:POLLING_RATE_SECONDS=5  # Polling interval" -ForegroundColor White
Write-Host "  `$env:GITHUB_REPO='user/repo' # GitHub repository for database sync" -ForegroundColor White
