#!/bin/bash

set -e

echo "DUW Collector Cross-Platform Build Script"
echo "========================================"

# Detect operating system
case "$(uname -s)" in
    Linux*)
        echo "Detected: Linux"
        echo "Running Linux build script..."
        ./scripts/build-linux.sh
        ;;
    Darwin*)
        echo "Detected: macOS"
        echo "Running macOS build script..."
        ./scripts/build-macos.sh
        ;;
    CYGWIN*|MINGW32*|MSYS*|MINGW*)
        echo "Detected: Windows (Git Bash/MSYS2)"
        echo "Please use PowerShell script for Windows:"
        echo "  powershell -ExecutionPolicy Bypass -File scripts/build-windows.ps1"
        echo ""
        echo "Or use the batch file:"
        echo "  scripts/build-windows.bat"
        exit 1
        ;;
    *)
        echo "Unknown operating system: $(uname -s)"
        echo "Please use the appropriate build script manually:"
        echo "  Linux:   ./scripts/build-linux.sh"
        echo "  macOS:   ./scripts/build-macos.sh"
        echo "  Windows: powershell -ExecutionPolicy Bypass -File scripts/build-windows.ps1"
        exit 1
        ;;
esac
