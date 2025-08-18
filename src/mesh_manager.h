#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <string_view>
#include <optional>
#include <chrono>
#include <cstdint>

class MeshManager {
private:
    bool isInitialized{false};
    
public:
    MeshManager() = default;
    ~MeshManager() = default;
    
    // Delete copy, allow move
    MeshManager(const MeshManager&) = delete;
    MeshManager& operator=(const MeshManager&) = delete;
    MeshManager(MeshManager&&) = default;
    MeshManager& operator=(MeshManager&&) = default;
    
    [[nodiscard]] bool init() noexcept;
    
    // Modern parameter types
    [[nodiscard]] bool sendSensorData(
        std::string_view wallId, 
        std::size_t deviceCount, 
        std::chrono::milliseconds timestamp
    ) noexcept;
    
    void handleIncomingMessages() noexcept;
    [[nodiscard]] bool isConnected() const noexcept;
    void printNodeInfo() const noexcept;
    
    // Modern utility methods
    [[nodiscard]] std::optional<std::string> getNodeId() const noexcept;
    [[nodiscard]] auto getLastTransmissionTime() const noexcept -> std::chrono::milliseconds;
};
