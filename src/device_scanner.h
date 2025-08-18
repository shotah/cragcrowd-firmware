#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>

// Modern C++20 includes
#include <unordered_set>
#include <memory>
#include <string_view>
#include <optional>
#include <chrono>

class DeviceScanner {
private:
    // Use unordered_set for O(1) lookups vs O(log n) for set
    std::unordered_set<std::string> detectedDevices;
    std::unordered_set<std::string> wifiDevices;
    std::unordered_set<std::string> bleDevices;
    BLEScan* pBLEScan; // TODO: Convert to smart pointer when BLE library supports it
    
    // Paxcounter-inspired settings
    bool climbingModeEnabled{false};
    
public:
    DeviceScanner() = default;
    ~DeviceScanner() = default;
    
    // Delete copy constructor/assignment (singleton-like behavior)
    DeviceScanner(const DeviceScanner&) = delete;
    DeviceScanner& operator=(const DeviceScanner&) = delete;
    
    // Allow move constructor/assignment
    DeviceScanner(DeviceScanner&&) = default;
    DeviceScanner& operator=(DeviceScanner&&) = default;
    
    [[nodiscard]] bool init() noexcept;
    void scanForDevices();
    [[nodiscard]] std::size_t getDeviceCount() const noexcept;
    void clearDevices() noexcept;
    
    // Modern accessor for detected devices
    [[nodiscard]] const auto& getDetectedDevices() const noexcept { 
        return detectedDevices; 
    }
    
    // Paxcounter-inspired analytics
    [[nodiscard]] std::size_t getUniqueDeviceCount() const noexcept;
    [[nodiscard]] std::size_t getWiFiDeviceCount() const noexcept;
    [[nodiscard]] std::size_t getBLEDeviceCount() const noexcept;
    
    // Advanced filtering for climbing areas
    void enableClimbingModeFiltering() noexcept;
    [[nodiscard]] std::optional<std::size_t> estimateHumanCount() const noexcept;
};

// Modern BLE callback class with safer reference semantics
class BLECallback : public BLEAdvertisedDeviceCallbacks {
public:
    // Use reference instead of pointer for non-null guarantee
    explicit BLECallback(std::unordered_set<std::string>& devices) noexcept 
        : detectedDevices(devices) {}
    
    // Override with modern noexcept specification
    void onResult(BLEAdvertisedDevice advertisedDevice) override;
    
    // Rule of 5: explicitly delete/default special functions
    ~BLECallback() = default;
    BLECallback(const BLECallback&) = delete;
    BLECallback& operator=(const BLECallback&) = delete;
    BLECallback(BLECallback&&) = delete;
    BLECallback& operator=(BLECallback&&) = delete;
    
private:
    std::unordered_set<std::string>& detectedDevices;
};
