# CragCrowd Firmware Makefile

.PHONY: build upload monitor clean setup install-deps test debug info flash-erase

# Default target
all: build

# Install dependencies
install-deps:
	@echo "Installing PlatformIO dependencies..."
	pio pkg install

# Build the firmware
build:
	@echo "Building CragCrowd firmware..."
	pio run

# Upload firmware to device
upload:
	@echo "Uploading firmware to device..."
	pio run --target upload

# Monitor serial output
monitor:
	@echo "Starting serial monitor..."
	pio device monitor

# Upload and monitor in one command
deploy: upload monitor

# Run tests (if available)
test:
	@echo "Running tests..."
	pio test

# Debug build with verbose output
debug:
	@echo "Building with debug information..."
	pio run -v

# Show build info and environment details
info:
	@echo "=== PlatformIO Environment Info ==="
	pio system info
	@echo "=== Board Configuration ==="
	pio boards tlora-t3s3-v1

# Erase flash completely (useful for troubleshooting)
flash-erase:
	@echo "Erasing flash memory..."
	pio run --target erase

# Clean build files
clean:
	@echo "Cleaning build files..."
	pio run --target clean

# Setup development environment
setup:
	@echo "Setting up development environment..."
	@echo "Installing PlatformIO if not present..."
	@which pio > /dev/null || pip install platformio
	$(MAKE) install-deps

# Check device connection
check-device:
	@echo "Checking connected devices..."
	pio device list

# Format code (if clang-format is available)
format:
	@echo "Formatting code..."
	@find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i || echo "clang-format not available"

# Quick development cycle: clean, build, upload, monitor
quick-dev: clean build deploy

# Build for different environments (if you add more later)
build-debug:
	@echo "Building debug configuration..."
	pio run -e tlora-t3s3-v1

# Monitor with specific baud rate
monitor-fast:
	@echo "Starting serial monitor at 921600 baud..."
	pio device monitor -b 921600

# Show compiled size information
size:
	@echo "Showing firmware size information..."
	pio run --target size

# Update PlatformIO and all dependencies
update:
	@echo "Updating PlatformIO and dependencies..."
	pio pkg update -g
	pio platform update

help:
	@echo "Available commands:"
	@echo "  setup         - Setup development environment"
	@echo "  build         - Build firmware"
	@echo "  upload        - Upload firmware to device"
	@echo "  monitor       - Monitor serial output"
	@echo "  deploy        - Upload and monitor"
	@echo "  clean         - Clean build files"
	@echo "  test          - Run tests"
	@echo "  debug         - Build with verbose output"
	@echo "  info          - Show environment and board info"
	@echo "  flash-erase   - Completely erase flash memory"
	@echo "  check-device  - List connected devices"
	@echo "  format        - Format source code"
	@echo "  quick-dev     - Clean, build, upload, monitor"
	@echo "  monitor-fast  - Monitor at 921600 baud"
	@echo "  size          - Show firmware size information"
	@echo "  update        - Update PlatformIO and dependencies"