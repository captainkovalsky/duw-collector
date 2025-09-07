#!/bin/bash

set -e

echo "Building DUW Collector for macOS..."

if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is not installed. Please install cmake first."
    echo "You can install it with: brew install cmake"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "Error: make is not installed. Please install make first."
    echo "You can install it with: brew install make"
    exit 1
fi

if ! command -v curl &> /dev/null; then
    echo "Error: curl development libraries are not installed. Please install curl first."
    echo "You can install it with: brew install curl"
    exit 1
fi

if ! command -v sqlite3 &> /dev/null; then
    echo "Error: sqlite3 development libraries are not installed. Please install sqlite3 first."
    echo "You can install it with: brew install sqlite3"
    exit 1
fi

mkdir -p build-macos
cd build-macos

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
make -j$(sysctl -n hw.ncpu)

echo "Build completed successfully!"
echo "Executable: build-macos/duw-collector"
echo ""
echo "To run:"
echo "  cd build-macos && ./duw-collector"
echo ""
echo "Environment variables:"
echo "  MODE=polling          # Run in polling mode"
echo "  DB_PATH=path/to/db   # Custom database path"
echo "  POLLING_RATE_SECONDS=5  # Polling interval"
echo "  GITHUB_REPO=user/repo # GitHub repository for database sync"
