# Build Scripts

This directory contains platform-specific build scripts for the DUW Collector application.

## Available Scripts

### Cross-Platform
- `build.sh` - Automatically detects the platform and runs the appropriate build script

### Platform-Specific
- `build-linux.sh` - Build script for Linux systems
- `build-macos.sh` - Build script for macOS systems
- `build-windows.bat` - Batch script for Windows (Command Prompt)
- `build-windows.ps1` - PowerShell script for Windows

## Prerequisites

### Linux
- cmake
- make
- g++ compiler
- libcurl4-openssl-dev (or equivalent curl development libraries)
- libsqlite3-dev (or equivalent sqlite3 development libraries)

Install on Ubuntu/Debian:
```bash
sudo apt update
sudo apt install cmake make g++ libcurl4-openssl-dev libsqlite3-dev
```

### macOS
- cmake
- make
- clang++ compiler (usually comes with Xcode Command Line Tools)
- curl (usually pre-installed)
- sqlite3 (usually pre-installed)

Install with Homebrew:
```bash
brew install cmake make curl sqlite3
```

### Windows
- Visual Studio 2022 or Visual Studio Build Tools
- cmake
- vcpkg (recommended for dependency management)

## Usage

### Quick Start (Cross-Platform)
```bash
./scripts/build.sh
```

### Platform-Specific

#### Linux
```bash
./scripts/build-linux.sh
```

#### macOS
```bash
./scripts/build-macos.sh
```

#### Windows (PowerShell)
```powershell
powershell -ExecutionPolicy Bypass -File scripts/build-windows.ps1
```

#### Windows (Command Prompt)
```cmd
scripts\build-windows.bat
```

## Environment Variables

The application supports the following environment variables:

- `MODE=polling` - Run in continuous polling mode
- `DB_PATH=path/to/db` - Custom database file path
- `POLLING_RATE_SECONDS=5` - Polling interval in seconds
- `GITHUB_REPO=user/repo` - GitHub repository for database synchronization

## Output

After successful build, the executable will be located in:
- Linux: `build-linux/duw-collector`
- macOS: `build-macos/duw-collector`
- Windows: `build-windows/Release/duw-collector.exe`

## Troubleshooting

### Common Issues

1. **Missing dependencies**: Ensure all required libraries are installed
2. **Permission denied**: Make sure the scripts are executable (`chmod +x scripts/*.sh`)
3. **CMake not found**: Install cmake and ensure it's in your PATH
4. **Compiler not found**: Install the appropriate compiler for your platform

### Getting Help

If you encounter issues:
1. Check that all prerequisites are installed
2. Verify the build script output for specific error messages
3. Ensure you have sufficient disk space for the build
4. Check that you're running the script from the project root directory
