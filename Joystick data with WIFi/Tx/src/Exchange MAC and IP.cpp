#include <WiFi.h>
#include <WiFiServer.h>

// Wi-Fi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Server details
const int serverPort = 8080;

WiFiServer server(serverPort);

// LED pins
const int ledXPin = 16; // Adjust to your first LED pin
const int ledYPin = 17; // Adjust to your second LED pin

typedef struct {
    uint8_t brightnessX;
    uint8_t brightnessY;
} DataPacket;

DataPacket receivedData;

void setup() {
    Serial.begin(115200);

    // Initialize LED pins
    pinMode(ledXPin, OUTPUT);
    pinMode(ledYPin, OUTPUT);

    // Initialize PWM for LEDs
    ledcSetup(0, 5000, 8); // Channel 0 for X LED
    ledcAttachPin(ledXPin, 0);

    ledcSetup(1, 5000, 8); // Channel 1 for Y LED
    ledcAttachPin(ledYPin, 1);

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

    // Start the server
    server.begin();
    Serial.printf("Server started on port %d\n", serverPort);
}

void loop() {
    // Check for a new client connection
    WiFiClient client = server.available();
    if (client) {
        Serial.println("Client connected");

        // Send the receiver's MAC address to the client
        uint8_t mac[6];
        WiFi.macAddress(mac);
        client.write(mac, 6);
        Serial.println("MAC address sent to client.");

        // Handle incoming data
        while (client.connected()) {
            if (client.available()) {
                // Read incoming data into the DataPacket structure
                int len = client.read((uint8_t*)&receivedData, sizeof(receivedData));
                if (len == sizeof(receivedData)) {
                    Serial.printf("Received X: %d, Y: %d\n", receivedData.brightnessX, receivedData.brightnessY);

                    // Adjust LED brightness
                    ledcWrite(0, receivedData.brightnessX); // Adjust X LED
                    ledcWrite(1, receivedData.brightnessY); // Adjust Y LED
                }
            }
        }
        client.stop();
        Serial.println("Client disconnected");
    }
}
