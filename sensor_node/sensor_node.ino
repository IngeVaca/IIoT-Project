/**
 * LoRa Sensor Node - Electrical Monitoring System
 * 
 * This node reads electrical parameters from PZEM-004T sensor
 * and transmits them via LoRa communication to the gateway.
 * 
 * Hardware: Heltec LoRa WiFi V3 + PZEM-004T
 * Function: Electrical parameter acquisition and wireless transmission
 */

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <PZEM004Tv30.h>

// ------------------- PZEM SENSOR CONFIGURATION -------------------
#define PZEM_RX_PIN 47  // UART RX pin for PZEM communication
#define PZEM_TX_PIN 48  // UART TX pin for PZEM communication
HardwareSerial PZEMSerial(2);  // Hardware serial port for PZEM
PZEM004Tv30 pzem(PZEMSerial, PZEM_RX_PIN, PZEM_TX_PIN);  // PZEM sensor object

// ------------------- LoRa COMMUNICATION CONFIGURATION -------------------
#define RF_FREQUENCY                915000000  // Operating frequency (915 MHz for LATAM/USA)
#define TX_OUTPUT_POWER             14         // Transmission power in dBm
#define LORA_BANDWIDTH              0          // 0 = 125 kHz bandwidth
#define LORA_SPREADING_FACTOR       7          // SF7 (Spreading Factor)
#define LORA_CODINGRATE             1          // Coding rate 4/5
#define LORA_PREAMBLE_LENGTH        8          // Preamble length
#define LORA_FIX_LENGTH_PAYLOAD_ON  false      // Variable length payload
#define LORA_IQ_INVERSION_ON        false      // IQ inversion disabled

// Transmission buffer and state variables
char txpacket[80];        // Buffer for LoRa transmission packet
bool lora_idle = true;    // LoRa state flag (true when ready to transmit)

// LoRa radio event handlers
static RadioEvents_t RadioEvents;

// Function prototypes
void OnTxDone(void);
void OnTxTimeout(void);

/**
 * Initialize the sensor node hardware and LoRa communication
 */
void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    
    // Initialize microcontroller unit (Heltec board specific)
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    // Configure LoRa radio event callbacks
    RadioEvents.TxDone = OnTxDone;        // Called when transmission completes
    RadioEvents.TxTimeout = OnTxTimeout;  // Called when transmission times out

    // Initialize LoRa radio with event handlers
    Radio.Init(&RadioEvents);
    
    // Set LoRa operating frequency
    Radio.SetChannel(RF_FREQUENCY);

    // Configure LoRa transmission parameters
    Radio.SetTxConfig(
        MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
        LORA_SPREADING_FACTOR, LORA_CODINGRATE,
        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
        true, 0, 0, LORA_IQ_INVERSION_ON, 3000
    );

    Serial.println("=== LoRa PZEM Transmitter Node Ready ===");
}

/**
 * Main program loop - continuously reads sensor data and transmits via LoRa
 */
void loop() {
    // Check if LoRa is ready to transmit new data
    if (lora_idle) {
        
        // Read electrical parameters from PZEM sensor
        float voltage = pzem.voltage();      // Voltage in volts
        float current = pzem.current();      // Current in amperes
        float power = pzem.power();          // Active power in watts
        float frequency = pzem.frequency();  // Frequency in Hz
        float powerFactor = pzem.pf();       // Power factor

        // Validate sensor readings
        if (isnan(voltage) || isnan(current) || isnan(power) || 
            isnan(frequency) || isnan(powerFactor)) {
            Serial.println("Error reading PZEM sensor data");
            delay(1000);
            return;
        }

        // Format sensor data into LoRa transmission packet
        // Data format: "V:230.5,I:1.25,P:288.1,F:60.0,PF:0.98"
        sprintf(txpacket, "V:%.1f,I:%.2f,P:%.1f,F:%.1f,PF:%.2f", 
                voltage, current, power, frequency, powerFactor);

        // Display transmission data for debugging
        Serial.printf("\nTransmitting: %s\n", txpacket);

        // Send data via LoRa radio
        Radio.Send((uint8_t*)txpacket, strlen(txpacket));

        // Set state to busy (waiting for transmission to complete)
        lora_idle = false;
    }

    // Process LoRa radio interrupts
    Radio.IrqProcess();
}

/**
 * Callback function - executed when LoRa transmission completes successfully
 */
void OnTxDone(void) {
    Serial.println("LoRa transmission completed successfully.");
    lora_idle = true;  // Set state back to idle (ready for next transmission)
}

/**
 * Callback function - executed when LoRa transmission times out
 */
void OnTxTimeout(void) {
    Serial.println("LoRa transmission timeout occurred.");
    lora_idle = true;  // Set state back to idle (ready for retry)
}
