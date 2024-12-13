#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Define constants for digital pin numbers
const int carButtonPin = 36;
const int armButtonPin = 39;

// Joystick data structure
typedef struct {
    int16_t carX;
    int16_t carY;
    int16_t armX;
    int16_t armY;
    bool carButton; // Add carButton to match usage in loop
    bool armButton; // Add armButton to match usage in loop
} JoystickData;

// Define constants for analog pin numbers
const int CAR_X_PIN = 34;
const int CAR_Y_PIN = 35;
const int ARM_X_PIN = 32;
const int ARM_Y_PIN = 33;

JoystickData joystickData;

// MAC address of the receiver
uint8_t receiverMac[] = {0xD0, 0xEF, 0x76, 0xEF, 0xDF, 0x70}; // Replace with actual MAC address

// ESP-NOW send callback function
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Data send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);

    // Initialize Wi-Fi in station mode
    WiFi.mode(WIFI_STA);
    Serial.println(WiFi.macAddress());

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register the send callback function
    esp_now_register_send_cb(onDataSent);

    // Add the receiver peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMac, 6);
    peerInfo.channel = 0;  // Match the receiver's channel
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    // Print receiver MAC for verification
    Serial.printf("Receiver MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  receiverMac[0], receiverMac[1], receiverMac[2],
                  receiverMac[3], receiverMac[4], receiverMac[5]);

    Serial.println("Transmitter ready.");

    // Configure button pins as input with internal pull-up resistors
    pinMode(carButtonPin, INPUT_PULLUP);
    pinMode(armButtonPin, INPUT_PULLUP);
}

void loop() {
    // Example joystick data (Replace with actual joystick readings)
    joystickData.carX = analogRead(CAR_X_PIN);
    joystickData.carY = analogRead(CAR_Y_PIN);
    joystickData.armX = analogRead(ARM_X_PIN);
    joystickData.armY = analogRead(ARM_Y_PIN);

    // Read button states for car and arm
    joystickData.carButton = digitalRead(carButtonPin) == LOW; // Assuming active low button
    joystickData.armButton = digitalRead(armButtonPin) == LOW; // Assuming active low button

    // Print joystick values to Serial for debugging
    Serial.printf("carX: %d, carY: %d, armX: %d, armY: %d, carButton: %d, armButton: %d\n",
                  joystickData.carX, joystickData.carY,
                  joystickData.armX, joystickData.armY,
                  joystickData.carButton, joystickData.armButton);

    // Send joystick data to receiver
    static unsigned long lastSendTime = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - lastSendTime >= 100) {
        lastSendTime = currentMillis;
        esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&joystickData, sizeof(joystickData));

        if (result == ESP_OK) {
            Serial.println("Joystick data sent successfully.");
        } else {
            Serial.printf("Error sending joystick data. Error code: %d\n", result);
        }
    }

    // Delay to avoid flooding the receiver
    delay(100);
}
