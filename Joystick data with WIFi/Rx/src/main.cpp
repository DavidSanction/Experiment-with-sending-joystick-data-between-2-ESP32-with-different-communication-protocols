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
const int buttonPin = 21; // Button

bool macAddressReceived = false;
uint8_t receiverMAC[6];

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

    // Display MAC address
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void loop() {
    // Connect to the receiver if not already connected
    if (!client.connected()) {
        Serial.println("Connecting to receiver...");
        if (client.connect(receiverIP, receiverPort)) {
            Serial.println("Connected to receiver.");

            // Wait for receiver to send its MAC address
            while (client.available() < 6) {
                delay(10);
            }
            client.read(receiverMAC, 6);
            macAddressReceived = true;

            Serial.printf("Receiver MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                          receiverMAC[0], receiverMAC[1], receiverMAC[2], receiverMAC[3], receiverMAC[4], receiverMAC[5]);
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
