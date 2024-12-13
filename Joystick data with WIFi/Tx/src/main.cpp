#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

// Wi-Fi credentials
const char* ssid = "WiFimodem-46A9";
const char* password = "kjnhjdqgz2";

// Receiver's IP and port
const char* receiverIP = "192.168.0.113"; // Replace with the receiver's IP address
const int receiverPort = 8080;

WiFiClient client;

typedef struct {
    uint8_t brightnessX;
    uint8_t brightnessY;
} DataPacket;

DataPacket dataToSend;

// Joystick and button pins
const int xPin = 36; // Joystick X-axis
const int yPin = 39; // Joystick Y-axis
const int buttonPin = 34; // Button

void setup() {
    Serial.begin(115200);

    // Initialize joystick and button pins
    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    // Initialize Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}

void loop() {
    // Connect to the receiver if not already connected
    if (!client.connected()) {
        Serial.println("Connecting to receiver...");
        if (client.connect(receiverIP, receiverPort)) {
            Serial.println("Connected to receiver.");
        } else {
            Serial.println("Failed to connect, retrying...");
            delay(1000);
            return;
        }
    }

    // Read joystick values and button state
    int xValue = analogRead(xPin) / 4; // Map 0-1023 to 0-255
    int yValue = analogRead(yPin) / 4; // Map 0-1023 to 0-255
    bool buttonPressed = digitalRead(buttonPin) == LOW;

    // Populate the data packet
    dataToSend.brightnessX = xValue;
    dataToSend.brightnessY = yValue;

    // Send data to the receiver
    if (client.connected()) {
        client.write((uint8_t*)&dataToSend, sizeof(dataToSend));
        Serial.printf("Sent X: %d, Y: %d, Button: %s\n", dataToSend.brightnessX, dataToSend.brightnessY, buttonPressed ? "Pressed" : "Released");
    } else {
        Serial.println("Disconnected, attempting to reconnect...");
    }

    delay(100); // Adjust transmission frequency as needed
}
