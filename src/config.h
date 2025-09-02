#pragma once

// Pin definitions for LilyGO T3S3 (SX1276 - 915MHz North America)
#define LORA_SCK  5
#define LORA_MISO 3
#define LORA_MOSI 6
#define LORA_SS   7
#define LORA_RST  8
#define LORA_DIO0 33  // DIO0 for SX1276
#define LORA_DIO1 -1  // Not connected on most T3S3 boards

// Configuration constants
constexpr int SCAN_DURATION = 30;  // seconds
constexpr int REPORT_INTERVAL = 10 * 60 * 1000; // 10 minutes in milliseconds

// Deployment configuration - CHANGE THESE FOR EACH SENSOR
constexpr const char* WALL_ID = "test_wall";  // Unique identifier for this sensor location

// Meshtastic configuration  
constexpr const char* MESH_NODE_NAME = "CragCrowd-Sensor";
constexpr int MESH_CHANNEL = 0;  // Default channel
