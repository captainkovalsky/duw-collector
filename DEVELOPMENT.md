# Development Setup Guide

This guide provides comprehensive instructions for setting up the development environment for the DUW Collector project.

## Prerequisites

### System Requirements
- **Operating System**: macOS or Linux (Ubuntu, CentOS, Fedora, Arch Linux)
- **C++ Compiler**: GCC 7+ or Clang 8+ with C++17 support
- **CMake**: Version 3.10 or higher
- **Git**: For version control

## Quick Setup

For a fully automated setup, run the toolchain setup script:

```bash
./scripts/setup_toolchain.sh
```

This script will:
- Detect your operating system
- Install all required dependencies
- Verify installations
- Set up the build environment
- Generate `compile_commands.json` for IDE support
- Test the build

## Manual Setup

If you prefer to set up dependencies manually, follow the OS-specific instructions below.

### macOS Setup

1. **Install Homebrew** (if not already installed):
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Install dependencies**:
   ```bash
   brew update
   brew install cmake pkg-config curl sqlite3 nlohmann-json clang-format
   ```

### Linux Setup

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libcurl4-openssl-dev \
    libsqlite3-dev \
    nlohmann-json3-dev \
    clang-format
```

#### CentOS/RHEL/Fedora
```bash
# For CentOS/RHEL
sudo yum install -y gcc-c++ cmake pkg-config libcurl-devel sqlite-devel json-devel clang-tools-extra

# For Fedora
sudo dnf install -y gcc-c++ cmake pkg-config libcurl-devel sqlite-devel json-devel clang-tools-extra
```

#### Arch Linux
```bash
sudo pacman -Syu --needed \
    base-devel \
    cmake \
    pkg-config \
    curl \
    sqlite \
    nlohmann-json \
    clang
```

## Building the Project

### Standard Build Process

1. **Create build directory**:
   ```bash
   mkdir build
   cd build
   ```

2. **Configure with CMake**:
   ```bash
   cmake ..
   ```

3. **Build the project**:
   ```bash
   make
   ```

4. **Run the application**:
   ```bash
   ./duw-collector
   ```

### Development Build

For development, enable compile commands export for better IDE support:

```bash
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
ln -sf build/compile_commands.json .  # Link to project root for clangd
make
```

## IDE and Editor Support

### Visual Studio Code
1. Install the C/C++ extension
2. Install the clangd extension (recommended over C/C++)
3. The `compile_commands.json` file provides full IntelliSense support

### CLion
CLion automatically imports CMake projects. Just open the project directory.

### Vim/Neovim with clangd
The `.clangd` configuration file is already set up for optimal development experience.

### Other Editors
Any editor supporting the Language Server Protocol (LSP) can use clangd with the provided configuration.

## Code Quality Tools

### Code Formatting
The project uses `clang-format` with the configuration in `.clang-format`:

```bash
# Format all source files
find src -name "*.cc" -o -name "*.h" | xargs clang-format -i
```

### Pre-commit Hooks
A pre-commit hook automatically formats code before commits. It's configured in `.git/hooks/pre-commit`.

### Static Analysis
The `.clangd` file enables various static analysis checks:
- Performance warnings
- Readability improvements
- Modern C++ suggestions

## Dependencies

### Core Dependencies
- **libcurl**: HTTP client library for fetching data
- **sqlite3**: Database storage
- **nlohmann/json**: Modern C++ JSON library

### Development Dependencies
- **clang-format**: Code formatting
- **clangd**: Language server for IDE support
- **pkg-config**: Library configuration management

## Environment Variables

The application supports the following environment variables:

- `POLLING_RATE_SECONDS`: Polling interval (default: 5)
- `DB_PATH`: Database file path (default: "duw_data.db")
- `MODE`: Operation mode - "single" or "polling" (default: "single")
- `GITHUB_REPO`: GitHub repository for database sync (format: "owner/repo")

Example:
```bash
export POLLING_RATE_SECONDS=10
export MODE=polling
./duw-collector
```

## Database Schema

The SQLite database contains a `ticket_info` table with the following structure:

```sql
CREATE TABLE ticket_info (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    city TEXT NOT NULL,
    service_name TEXT NOT NULL,
    service_id INTEGER,
    queue_status TEXT NOT NULL,
    queue_length INTEGER NOT NULL,
    operations_count INTEGER DEFAULT 0,
    enabled_operations INTEGER DEFAULT 0,
    timestamp TEXT NOT NULL
);
```

## Testing the Setup

### Verify Dependencies
```bash
# Check CMake
cmake --version

# Check pkg-config
pkg-config --version

# Check curl development headers
pkg-config --exists libcurl && echo "libcurl OK"

# Check SQLite3 development headers
pkg-config --exists sqlite3 && echo "sqlite3 OK"

# Check nlohmann/json (macOS)
brew list nlohmann-json

# Check nlohmann/json (Linux)
find /usr/include -name nlohmann 2>/dev/null
```

### Build Test
```bash
cd build
make clean
make
./duw-collector
```

The application should:
1. Initialize successfully
2. Fetch data from the DUW API
3. Parse JSON response
4. Save data to SQLite database
5. Exit with code 0

## Troubleshooting

### Common Issues

#### CMake can't find nlohmann/json
**Solution**: Ensure the library is properly installed:
```bash
# macOS
brew install nlohmann-json

# Ubuntu/Debian
sudo apt-get install nlohmann-json3-dev
```

#### Missing development headers
**Error**: `fatal error: 'curl/curl.h' file not found`
**Solution**: Install development packages:
```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev libsqlite3-dev

# CentOS/RHEL
sudo yum install libcurl-devel sqlite-devel
```

#### clangd not finding headers
**Solution**: Ensure `compile_commands.json` is generated and linked:
```bash
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cd ..
ln -sf build/compile_commands.json .
```

#### Permission denied on script execution
**Solution**: Make the script executable:
```bash
chmod +x scripts/setup_toolchain.sh
```

## Contributing

### Code Style
- Follow the existing code style
- Use `clang-format` for formatting
- Follow the guidelines in `agent/CODE_CONVENTIONS.md`

### Commit Process
1. Make changes
2. Format code (automatic via pre-commit hook)
3. Test the build
4. Commit with descriptive messages

### Adding Dependencies
When adding new dependencies:
1. Update `CMakeLists.txt`
2. Update `setup_toolchain.sh`
3. Update this documentation
4. Test on multiple platforms

## Project Structure

```
duw-collector/
├── src/                    # Source code
│   ├── main.cc            # Application entry point
│   ├── collector.{cc,h}   # Main collector logic
│   ├── http_client.{cc,h} # HTTP client
│   ├── database_service.{cc,h}  # Database operations
│   ├── db_connection.{cc,h} # Database connection RAII wrapper
│   ├── env_service.{cc,h} # Environment variable handling
│   ├── json_parser.{cc,h} # JSON parsing
│   └── github_service.{cc,h} # GitHub integration
├── build/                 # Build directory (git-ignored)
├── .clang-format         # Code formatting configuration
├── .clangd              # Language server configuration
├── .gitignore           # Git ignore rules
├── CMakeLists.txt       # CMake configuration
├── scripts/             # Build and setup scripts
│   ├── setup_toolchain.sh   # Automated setup script
│   ├── build.sh             # Cross-platform build script
│   ├── build-linux.sh       # Linux build script
│   ├── build-macos.sh       # macOS build script
│   ├── build-windows.bat    # Windows batch build script
│   ├── build-windows.ps1    # Windows PowerShell build script
│   └── README.md            # Scripts documentation
├── agent/               # Agent workflow files
│   ├── CODE_CONVENTIONS.md  # Coding standards
│   ├── AGENT_WORKFLOW.md    # Agent workflow
│   └── VERIFY.md           # Verification requirements
├── DEVELOPMENT.md       # This file
└── tasks.todo          # Project tasks
```

## Performance Notes

- **nlohmann/json**: High-performance JSON parsing with modern C++ API
- **SQLite**: Embedded database with excellent performance for local storage
- **libcurl**: Efficient HTTP client with connection reuse
- **Compilation**: Optimized builds with `-O2` or `-O3` for production

## Security Considerations

- No hardcoded credentials
- Environment variables for configuration
- Input validation on all external data
- Safe string handling practices
- Memory-safe C++ patterns

For additional help or questions, refer to the project's issue tracker or documentation.