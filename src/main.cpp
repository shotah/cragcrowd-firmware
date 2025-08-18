#include <Arduino.h>
#include "config.h"
#include "device_scanner.h"
#include "mesh_manager.h"
#include <chrono>

// Configuration variable definitions
const char* WALL_ID = "test_wall"; // This should be configurable
const char* MESH_NODE_NAME = "CragCrowd-Sensor";

// Global instances
DeviceScanner scanner;
MeshManager meshManager;

// Modern time management with chrono
using namespace std::chrono;
auto lastReportTime = milliseconds{0};

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("CragCrowd Mesh Sensor Node Starting...");
    Serial.println("=== PRIMARY MISSION ===");
    Serial.println("Deploy sensors beyond gateway range using 3rd party mesh nodes");
    Serial.println("========================");
    
    // Initialize device scanner with modern error handling
    if (!scanner.init()) {
        Serial.println("FATAL: Failed to initialize device scanner!");
        while (true) {
            delay(1000);
        }
    }
    
    // Enable climbing-specific optimizations
    scanner.enableClimbingModeFiltering();
    
    // Initialize mesh networking
    if (!meshManager.init()) {
        Serial.println("FATAL: Failed to initialize mesh networking!");
        while (true) {
            delay(1000);
        }
    }
    
    // Print node information
    meshManager.printNodeInfo();
    
    Serial.println("Setup complete. Starting mesh monitoring...");
}

void loop() {
    // Modern time handling with chrono
    const auto currentTime = milliseconds{millis()};
    const auto reportInterval = milliseconds{REPORT_INTERVAL};
    
    // Handle incoming mesh messages
    meshManager.handleIncomingMessages();
    
    // Periodic sensor scanning and reporting
    if (currentTime - lastReportTime >= reportInterval) {
        Serial.println("Starting sensor scan...");
        
        // Scan for devices
        scanner.scanForDevices();
        
        // Enhanced analytics reporting
        const auto estimatedPeople = scanner.estimateHumanCount();
        if (estimatedPeople.has_value()) {
            Serial.printf("Estimated climbers: %zu people\n", estimatedPeople.value());
        }
        
        // Send data via mesh network with modern types
        const bool success = meshManager.sendSensorData(
            std::string_view{WALL_ID}, 
            scanner.getDeviceCount(), 
            currentTime
        );
        
        // Modern conditional output
        Serial.println(success ? "Mesh transmission successful" : "Mesh transmission failed");
        
        lastReportTime = currentTime;
    }
    
    delay(1000);
}