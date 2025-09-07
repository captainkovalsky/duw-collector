@echo off
setlocal

echo Building DUW Collector for Windows...

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: cmake is not installed. Please install cmake first.
    echo You can download it from: https://cmake.org/download/
    exit /b 1
)

where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: Visual Studio Build Tools are not installed or not in PATH.
    echo Please install Visual Studio Build Tools or Visual Studio Community.
    exit /b 1
)

if not exist "C:\vcpkg" (
    echo Warning: vcpkg not found. You may need to install dependencies manually.
    echo Consider installing vcpkg for easier dependency management.
)

if not exist build-windows mkdir build-windows
cd build-windows

cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

echo Build completed successfully!
echo Executable: build-windows\Release\duw-collector.exe
echo.
echo To run:
echo   cd build-windows\Release && duw-collector.exe
echo.
echo Environment variables:
echo   set MODE=polling          # Run in polling mode
echo   set DB_PATH=path\to\db   # Custom database path
echo   set POLLING_RATE_SECONDS=5  # Polling interval
echo   set GITHUB_REPO=user/repo # GitHub repository for database sync
