#include "device_scanner.h"
#include "config.h"
#include <esp_wifi.h>
#include <esp_bt.h>
#include <algorithm>

void BLECallback::onResult(BLEAdvertisedDevice advertisedDevice) {
    // Convert to std::string for modern container
    std::string macAddress = advertisedDevice.getAddress().toString().c_str();
    detectedDevices.insert(macAddress);
    // Note: We'll need to access the scanner instance to update bleDevices
    // This is a limitation of the current callback design
}

bool DeviceScanner::init() noexcept {
    try {
        // Setup WiFi scanning
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        
        // Setup BLE scanning
        BLEDevice::init("");
        pBLEScan = BLEDevice::getScan();
        if (!pBLEScan) {
            Serial.println("ERROR: Failed to get BLE scan object");
            return false;
        }
        
        // Use modern callback with reference semantics
        pBLEScan->setAdvertisedDeviceCallbacks(new BLECallback(detectedDevices));
        pBLEScan->setActiveScan(true);
        pBLEScan->setInterval(100);
        pBLEScan->setWindow(99);
        
        Serial.println("Device scanner initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        Serial.printf("ERROR: DeviceScanner init failed: %s\n", e.what());
        return false;
    } catch (...) {
        Serial.println("ERROR: DeviceScanner init failed with unknown exception");
        return false;
    }
}

void DeviceScanner::scanForDevices() {
    // Clear previous scan results
    detectedDevices.clear();
    wifiDevices.clear();
    bleDevices.clear();
    
    // Scan WiFi networks with modern range-based approach
    const int networkCount = WiFi.scanNetworks();
    if (networkCount > 0) {
        for (int i = 0; i < networkCount; ++i) {
            std::string bssid = WiFi.BSSIDstr(i).c_str();
            wifiDevices.emplace(bssid);
            detectedDevices.emplace(std::move(bssid));
        }
    }
    
    // Scan BLE devices
    if (pBLEScan) {
        const auto bleStartSize = detectedDevices.size();
        [[maybe_unused]] BLEScanResults foundDevices = pBLEScan->start(SCAN_DURATION, false);
        pBLEScan->clearResults();
        
        // Calculate BLE devices (total - wifi = ble)
        // This is a workaround for the callback limitation
        const auto totalDevices = detectedDevices.size();
        const auto bleCount = totalDevices - bleStartSize;
        Serial.printf("BLE scan found %zu new devices\n", bleCount);
    }
    
    // Modern paxcounter-style reporting
    Serial.printf("WiFi: %zu, BLE: %zu, Total: %zu unique devices\n", 
                  wifiDevices.size(), 
                  detectedDevices.size() - wifiDevices.size(),
                  detectedDevices.size());
}

std::size_t DeviceScanner::getDeviceCount() const noexcept {
    return detectedDevices.size();
}

void DeviceScanner::clearDevices() noexcept {
    detectedDevices.clear();
    wifiDevices.clear();
    bleDevices.clear();
}

// Paxcounter-inspired analytics methods
std::size_t DeviceScanner::getUniqueDeviceCount() const noexcept {
    return detectedDevices.size();
}

std::size_t DeviceScanner::getWiFiDeviceCount() const noexcept {
    return wifiDevices.size();
}

std::size_t DeviceScanner::getBLEDeviceCount() const noexcept {
    return detectedDevices.size() - wifiDevices.size();
}

void DeviceScanner::enableClimbingModeFiltering() noexcept {
    climbingModeEnabled = true;
    Serial.println("Climbing mode filtering enabled - optimized for outdoor recreation areas");
}

std::optional<std::size_t> DeviceScanner::estimateHumanCount() const noexcept {
    if (!climbingModeEnabled) {
        return std::nullopt;
    }
    
    // Climbing-specific estimation algorithm
    const auto wifiCount = getWiFiDeviceCount();
    const auto bleCount = getBLEDeviceCount();
    
    // Heuristic: Most climbers have 1-2 devices (phone + watch/earbuds)
    // WiFi devices are more reliable indicators of actual people
    // BLE devices include many accessories but some phantom devices
    
    const auto estimatedPeople = std::max(
        wifiCount,  // Minimum: one person per unique WiFi device
        static_cast<std::size_t>((wifiCount + bleCount) * 0.6)  // Conservative estimate
    );
    
    return estimatedPeople;
}
