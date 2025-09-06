#!/bin/bash

set -e

echo "=== DUW Collector Toolchain Setup ==="
echo

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
    else
        print_error "Unsupported operating system: $OSTYPE"
        exit 1
    fi
    print_status "Detected OS: $OS"
}

command_exists() {
    command -v "$1" >/dev/null 2>&1
}

install_macos_deps() {
    print_status "Installing macOS dependencies..."
    
    if ! command_exists brew; then
        print_status "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    else
        print_success "Homebrew already installed"
    fi
    
    print_status "Installing packages with Homebrew..."
    brew update
    brew install cmake pkg-config curl sqlite3 nlohmann-json clang-format
    
    print_success "macOS dependencies installed"
}

install_linux_deps() {
    print_status "Installing Linux dependencies..."
    
    if command_exists apt-get; then
        print_status "Using apt-get (Ubuntu/Debian)..."
        sudo apt-get update
        sudo apt-get install -y build-essential cmake pkg-config libcurl4-openssl-dev libsqlite3-dev nlohmann-json3-dev clang-format
    elif command_exists yum; then
        print_status "Using yum (CentOS/RHEL)..."
        sudo yum install -y gcc-c++ cmake pkg-config libcurl-devel sqlite-devel json-devel clang-tools-extra
    elif command_exists dnf; then
        print_status "Using dnf (Fedora)..."
        sudo dnf install -y gcc-c++ cmake pkg-config libcurl-devel sqlite-devel json-devel clang-tools-extra
    elif command_exists pacman; then
        print_status "Using pacman (Arch Linux)..."
        sudo pacman -Syu --needed base-devel cmake pkg-config curl sqlite nlohmann-json clang
    else
        print_error "Unsupported Linux distribution. Please install dependencies manually."
        exit 1
    fi
    
    print_success "Linux dependencies installed"
}

verify_installations() {
    print_status "Verifying installations..."
    
    local tools=(
        "cmake:CMake"
        "pkg-config:pkg-config"
        "clang-format:clang-format"
    )
    
    local all_good=true
    
    for tool_info in "${tools[@]}"; do
        IFS=':' read -r cmd name <<< "$tool_info"
        if command_exists "$cmd"; then
            version=$($cmd --version | head -n1)
            print_success "$name: $version"
        else
            print_error "$name is not installed or not in PATH"
            all_good=false
        fi
    done
    
    if [ "$all_good" = false ]; then
        print_error "Some tools are missing. Please install them manually."
        exit 1
    fi
    
    print_success "All required tools are installed"
}

setup_build_env() {
    print_status "Setting up build environment..."
    
    if [ ! -d "build" ]; then
        mkdir build
        print_success "Created build directory"
    else
        print_success "Build directory already exists"
    fi
    
    cd build
    
    if [ ! -f "CMakeCache.txt" ]; then
        print_status "Configuring CMake..."
        cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
        print_success "CMake configuration completed"
    else
        print_success "CMake already configured"
    fi
    
    cd ..
    
    if [ ! -f "compile_commands.json" ]; then
        ln -sf build/compile_commands.json .
        print_success "Linked compile_commands.json for IDE support"
    else
        print_success "compile_commands.json already linked"
    fi
}

test_build() {
    print_status "Testing build..."
    
    local original_dir=$(pwd)
    
    if [ -d "build" ]; then
        cd build || {
            print_error "Failed to enter build directory"
            exit 1
        }
        
        if make; then
            print_success "Build completed successfully!"
            
            print_status "Testing application execution..."
            if ./duw-collector >/dev/null 2>&1; then
                print_success "Application runs successfully!"
            else
                print_warning "Application may have runtime issues, but compilation succeeded"
            fi
        else
            print_error "Build failed!"
            cd "$original_dir"
            exit 1
        fi
        cd "$original_dir"
    else
        print_error "Build directory not found at: $(pwd)/build"
        exit 1
    fi
}

main() {
    print_status "Starting toolchain setup for DUW Collector..."
    echo
    
    detect_os
    
    case $OS in
        "macos")
            install_macos_deps
            ;;
        "linux")
            install_linux_deps
            ;;
    esac
    
    echo
    verify_installations
    echo
    setup_build_env
    echo
    test_build
    echo
    
    print_success "🎉 Toolchain setup completed successfully!"
    print_status "You can now build the project with:"
    print_status "  cd build && make"
    print_status ""
    print_status "Or run the application directly:"
    print_status "  cd build && ./duw-collector"
}

main "$@"