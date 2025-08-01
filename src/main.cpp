#include <Arduino.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <esp_wifi.h>
#include <esp_bt.h>

// Pin definitions for LilyGO T3S3
#define LORA_SCK  5
#define LORA_MISO 3
#define LORA_MOSI 6
#define LORA_SS   7
#define LORA_RST  8
#define LORA_DIO0 33

// Configuration
const int SCAN_DURATION = 30;  // seconds
const int REPORT_INTERVAL = 10 * 60 * 1000; // 10 minutes in milliseconds
const char* WALL_ID = "test_wall"; // This should be configurable

// Device tracking
std::set<String> detectedDevices;
unsigned long lastReportTime = 0;

class BLECallback : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        String macAddress = advertisedDevice.getAddress().toString().c_str();
        detectedDevices.insert(macAddress);
    }
};

void setupLoRa() {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    
    if (!LoRa.begin(915E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    
    Serial.println("LoRa Initializing OK!");
}

void setupWiFiScan() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

void setupBLEScan() {
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new BLECallback());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void scanForDevices() {
    // Clear previous scan results
    detectedDevices.clear();
    
    // Scan WiFi networks
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        String bssid = WiFi.BSSIDstr(i);
        detectedDevices.insert(bssid);
    }
    
    // Scan BLE devices
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults foundDevices = pBLEScan->start(SCAN_DURATION, false);
    pBLEScan->clearResults();
    
    Serial.printf("Detected %d unique devices\\n", detectedDevices.size());
}

void sendReport() {
    DynamicJsonDocument doc(1024);
    doc["wall_id"] = WALL_ID;
    doc["device_count"] = detectedDevices.size();
    doc["timestamp"] = millis();
    
    String message;
    serializeJson(doc, message);
    
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    
    Serial.printf("Sent report: %s\\n", message.c_str());
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("CragCrowd Sensor Node Starting...");
    
    setupLoRa();
    setupWiFiScan();
    setupBLEScan();
    
    Serial.println("Setup complete. Starting monitoring...");
}

void loop() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastReportTime >= REPORT_INTERVAL) {
        scanForDevices();
        sendReport();
        lastReportTime = currentTime;
    }
    
    delay(1000);
}