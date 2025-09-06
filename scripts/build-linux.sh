#!/bin/bash

set -e

echo "Building DUW Collector for Linux..."

if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is not installed. Please install cmake first."
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "Error: make is not installed. Please install make first."
    exit 1
fi

if ! command -v curl &> /dev/null; then
    echo "Error: curl development libraries are not installed. Please install libcurl4-openssl-dev or equivalent."
    exit 1
fi

if ! command -v sqlite3 &> /dev/null; then
    echo "Error: sqlite3 development libraries are not installed. Please install libsqlite3-dev or equivalent."
    exit 1
fi

mkdir -p build-linux
cd build-linux

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
make -j$(nproc)

echo "Build completed successfully!"
echo "Executable: build-linux/duw-collector"
echo ""
echo "To run:"
echo "  cd build-linux && ./duw-collector"
echo ""
echo "Environment variables:"
echo "  MODE=polling          # Run in polling mode"
echo "  DB_PATH=path/to/db   # Custom database path"
echo "  POLLING_RATE_SECONDS=5  # Polling interval"
echo "  GITHUB_REPO=user/repo # GitHub repository for database sync"
