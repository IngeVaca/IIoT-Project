/**
 * LoRa Gateway - IoT Electrical Monitoring System
 * 
 * This gateway receives electrical data from sensor nodes via LoRa,
 * processes the measurements, converts to JSON format, and forwards
 * to MQTT broker for centralized distribution.
 * 
 * Hardware: Heltec LoRa WiFi V3
 * Function: Protocol conversion (LoRa to WiFi/MQTT)
 */

#include "LoRaWan_APP.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "Arduino.h"

// ------------ WIFI CONFIGURATION ------------
// WiFi credentials should be configured in the application
const char* ssid = "YOUR_WIFI_SSID";           // WiFi network name
const char* password = "YOUR_WIFI_PASSWORD";   // WiFi network password

WiFiClient espClient;              // ESP32 WiFi client instance
PubSubClient mqtt(espClient);      // MQTT client instance

// MQTT broker configuration - update with your broker IP
const char* mqtt_server = "192.168.1.100";     // MQTT broker IP address
const int mqtt_port = 1883;                    // MQTT broker port

// ------------ LoRa CONFIGURATION ------------
#define RF_FREQUENCY 915000000     // LoRa frequency (915MHz for Americas)

char rxpacket[80];                 // Buffer for received LoRa packets
bool lora_idle = true;             // LoRa state management flag

static RadioEvents_t RadioEvents;  // LoRa radio event handlers

// Function prototype for LoRa reception callback
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

// ------------ MQTT FUNCTIONS ------------
/**
 * Reconnect to MQTT broker if connection is lost
 * Retries until successful connection is established
 */
void reconnectMQTT() {
    while (!mqtt.connected()) {
        // Attempt connection with gateway identifier
        if (mqtt.connect("Heltec_Gateway")) {
            Serial.println("MQTT connection established");
            break;
        }
        delay(1000);  // Wait before retry
    }
}

/**
 * Initialize gateway hardware and communication protocols
 */
void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);

    // Connect to WiFi network
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    
    // Wait for WiFi connection with progress indication
    while (WiFi.status() != WL_CONNECTED) {
        delay(300);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected successfully");
    
    // Configure MQTT broker connection
    mqtt.setServer(mqtt_server, mqtt_port);

    // ------------ LoRa INITIALIZATION ------------
    // Initialize Heltec microcontroller unit
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    // Register LoRa event callback
    RadioEvents.RxDone = OnRxDone;
    
    // Initialize LoRa radio with event handlers
    Radio.Init(&RadioEvents);
    
    // Set LoRa operating frequency
    Radio.SetChannel(RF_FREQUENCY);

    // Configure LoRa reception parameters
    Radio.SetRxConfig(
        MODEM_LORA,     // LoRa modulation
        0,              // Bandwidth
        7,              // Spreading Factor (SF7)
        1,              // Coding Rate
        0,              // LowDataRateOptimize
        8,              // Preamble length
        0,              // SymbTimeout
        false,          // Fixed length payload
        0,              // Payload length
        true,           // CRC enabled
        0,              // Frequency hopping
        0,              // Hop period
        false,          // IQ inversion
        true            // Continuous reception mode
    );

    Serial.println("LoRa Gateway Ready - Listening for transmissions...");
}

/**
 * Main program loop - manages MQTT connection and LoRa reception
 */
void loop() {
    // Maintain MQTT connection
    if (!mqtt.connected()) {
        reconnectMQTT();
    }
    mqtt.loop();  // Process MQTT messages

    // Initiate LoRa reception when radio is idle
    if (lora_idle) {
        lora_idle = false;
        Radio.Rx(0);  // Start continuous reception
    }

    // Process LoRa radio interrupts
    Radio.IrqProcess();
}

/**
 * Callback function executed when LoRa packet is received
 * @param payload Received data buffer
 * @param size Data size in bytes
 * @param rssi Received signal strength indicator
 * @param snr Signal-to-noise ratio
 */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    // Copy received payload to buffer and null-terminate
    memcpy(rxpacket, payload, size);
    rxpacket[size] = '\0';

    // Reset reception state and put radio to sleep
    lora_idle = true;
    Radio.Sleep();

    // Display received data for monitoring
    Serial.print("Received LoRa packet -> ");
    Serial.println(rxpacket);

    // Parse electrical parameters from received string
    float voltage = 0, current = 0, power = 0, frequency = 0, powerFactor = 0;
    sscanf(rxpacket, "V:%f,I:%f,P:%f,F:%f,PF:%f", 
           &voltage, &current, &power, &frequency, &powerFactor);

    // Calculate derived electrical parameters
    float apparentPower = voltage * current;                   // S = V × I
    float reactivePower = sqrt((apparentPower * apparentPower) - (power * power));  // Q = √(S² - P²)

    // Create JSON payload with all electrical parameters and radio metrics
    String jsonPayload = "{";
    jsonPayload += "\"V\":" + String(voltage, 1) + ",";        // Voltage
    jsonPayload += "\"I\":" + String(current, 2) + ",";        // Current  
    jsonPayload += "\"P\":" + String(power, 1) + ",";          // Active Power
    jsonPayload += "\"F\":" + String(frequency, 1) + ",";      // Frequency
    jsonPayload += "\"PF\":" + String(powerFactor, 2) + ",";   // Power Factor
    jsonPayload += "\"S\":" + String(apparentPower, 1) + ",";  // Apparent Power
    jsonPayload += "\"Q\":" + String(reactivePower, 1) + ",";  // Reactive Power
    jsonPayload += "\"rssi\":" + String(rssi) + ",";           // LoRa signal strength
    jsonPayload += "\"snr\":" + String(snr);                   // LoRa signal quality
    jsonPayload += "}";

    // Publish consolidated JSON message to MQTT broker
    bool published = mqtt.publish("sensor/electrical/data", jsonPayload.c_str());

    // Confirm successful publication
    if (published) {
        Serial.println("JSON data published to MQTT broker");
        Serial.println("Data content: " + jsonPayload);
    } else {
        Serial.println("Error publishing data to MQTT broker");
    }
}
