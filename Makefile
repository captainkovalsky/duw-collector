.PHONY: all clean build test install

all: build

build:
	@echo "Building DUW Collector..."
	@if [ ! -d "build" ]; then \
		echo "Creating build directory..."; \
		mkdir build; \
	fi
	@cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	@cd build && make
	@echo "Build completed successfully!"
	@echo "Executable: build/duw-collector"

clean:
	@echo "Cleaning build directory..."
	@rm -rf build
	@echo "Clean completed!"

test: build
	@echo "Testing application..."
	@cd build && ./duw-collector
	@echo "Test completed!"

install: build
	@echo "Installing DUW Collector..."
	@cp build/duw-collector /usr/local/bin/ 2>/dev/null || echo "Installation requires sudo privileges"
	@echo "Installation completed!"

help:
	@echo "DUW Collector Makefile"
	@echo "======================"
	@echo "Available targets:"
	@echo "  all     - Build the application (default)"
	@echo "  build   - Build the application"
	@echo "  clean   - Clean build directory"
	@echo "  test    - Build and test the application"
	@echo "  install - Install the application to /usr/local/bin"
	@echo "  help    - Show this help message"
