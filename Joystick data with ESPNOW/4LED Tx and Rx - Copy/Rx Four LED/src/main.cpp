#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// LED pins
#define LED1_PIN 16
#define LED2_PIN 17
#define LED3_PIN 18
#define LED4_PIN 19

// Define your peer information
esp_now_peer_info_t peerInfo;
int wifiChannel = 0; // Define the WiFi channel

// Callback function when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    Serial.print("Received data from: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(mac_addr[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.print(" | Data: ");
    
    // Assuming data contains 4 bytes, each byte representing the state of one LED
    if (data_len >= 4) {
        digitalWrite(LED1_PIN, data[0]);
        digitalWrite(LED2_PIN, data[1]);
        digitalWrite(LED3_PIN, data[2]);
        digitalWrite(LED4_PIN, data[3]);
    }
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Initialize WiFi
    WiFi.mode(WIFI_STA);
    Serial.println("WiFi initialized");

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    Serial.println("ESP-NOW initialized");

    // Register the receive callback
    esp_now_register_recv_cb(OnDataRecv);

    // Initialize LED pins
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED4_PIN, OUTPUT);
}

void loop() {
    // Add code here if needed
}