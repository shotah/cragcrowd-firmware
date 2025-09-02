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

# Prepare firmware for Windows upload
upload-windows: build
	@echo "=================================================="
	@echo "Copying firmware files to Windows temp directory..."
	@echo "=================================================="
	@mkdir -p /mnt/c/temp/cragcrowd-firmware
	@cp .pio/build/lilygo-t3-s3/firmware.bin /mnt/c/temp/cragcrowd-firmware/
	@cp .pio/build/lilygo-t3-s3/bootloader.bin /mnt/c/temp/cragcrowd-firmware/ 2>/dev/null || echo "Bootloader not found (normal for some builds)"
	@cp .pio/build/lilygo-t3-s3/partitions.bin /mnt/c/temp/cragcrowd-firmware/ 2>/dev/null || echo "Partitions not found (normal for some builds)"
	@echo ""
	@echo "Files copied to C:\\temp\\cragcrowd-firmware\\"
	@echo ""
	@echo "=================================================="
	@echo "Windows Upload Instructions:"
	@echo "=================================================="
	@echo "1. Put ESP32 in bootloader mode:"
	@echo "   - Hold BOOT button"
	@echo "   - Press and release RESET button"
	@echo "   - Release BOOT button"
	@echo ""
	@echo "2. Find your device in Windows PowerShell (Admin):"
	@echo "   usbipd list"
	@echo ""
	@echo "3. Run these commands in Windows PowerShell (Admin):"
	@echo "   cd C:\\temp\\cragcrowd-firmware"
	@echo "   python -m esptool --chip esp32s3 --port COM5 --baud 921600 write_flash 0x0 firmware.bin"
	@echo ""
	@echo "   (Replace COM5 with your actual COM port from Device Manager)"
	@echo ""
	@echo "4. Alternative with Arduino IDE:"
	@echo "   - Tools > Board > ESP32 Arduino > ESP32S3 Dev Module"
	@echo "   - Tools > Port > [Your COM Port]"
	@echo "   - Sketch > Upload Using Programmer"
	@echo "   - Select: C:\\temp\\cragcrowd-firmware\\firmware.bin"
	@echo ""
	@echo "5. After upload, press RESET button to run firmware"
	@echo "=================================================="

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