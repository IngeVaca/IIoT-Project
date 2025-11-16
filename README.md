# IoT Electrical Monitoring System

Distributed electrical monitoring system implementing a scalable IoT architecture for industrial applications. The solution utilizes two specialized Heltec LoRa WiFi V3 devices: the first operates as a sensor node that acquires electrical measurements from the PZEM-004T module and transmits them via LoRa communication, while the second functions as a gateway, receiving data through LoRa and forwarding it via WiFi to the MQTT broker for centralized processing.

This development was conceived and executed as an application project within the Master's program in Automatic Engineering and Cyber-Physical Systems, demonstrating the practical application of IoT concepts in the field of energy monitoring.

## System Architecture

| Layer | Component | Technology | Function | Communication |
|-------|-----------|------------|----------|---------------|
| **Sensing** | Sensor Node | Heltec + PZEM-004T | Electrical Parameter Measurement | LoRa Transmission |
| **Gateway** | Gateway | Heltec + WiFi | Data Reception & Protocol Conversion | LoRa → WiFi |
| **Communication** | MQTT Broker | EMQX (localhost) | Message Broker & Data Distribution | MQTT Protocol |
| **Presentation** | Web Dashboard | Single HTML File | Real-time Visualization & Control | Embedded CSS/JavaScript |
| **Infrastructure** | Network | Local WiFi | Local Communication | TCP/IP Network |

## Key Features

### Distributed Architecture
- **Decentralized Design**: Enables physical distribution of measurement points
- **Sensor Node**: Heltec with PZEM-004T that measures electrical parameters and transmits via LoRa
- **Gateway**: Heltec that receives LoRa data and forwards it via WiFi to MQTT
- **Scalability**: Designed to support multiple sensor nodes in the future

### Measurement Capabilities
- **Direct Parameters**: Tension, current, frequency, power factor
- **Derived Calculations**: Reactive and apparent power
- **Power Triangle**: Dynamic visualization of energy relationships
- **Real-time Acquisition**: Continuous monitoring of electrical parameters

### Communication & Integration
- **Long-range Wireless**: LoRa communication between nodes
- **Standard Protocols**: MQTT integration for data distribution
- **Network Flexibility**: Gateway can connect to different WiFi networks

### Web Dashboard
- **Unified Visualization**: Single HTML file with embedded CSS and JavaScript
- **Real-time Charts**: Interactive visualization using Chart.js library
- **MQTT Client**: Browser-based communication using Paho MQTT library
- **Data Management**: Configurable data capture with CSV export functionality

## Repository Structure

IoT-Electrical-Monitoring/

 ├── sensor_node/            Arduino code for Heltec sensor
 
 ├── gateway/                Arduino code for Heltec gateway
 
 ├── dashboard.html          Single HTML file with embedded CSS/JS
 
 ├── docs/                  Technical documentation and diagrams
 
 ├── examples/              Sample data and analysis
 
 └── proteus/               Electronic schematics


## Setup and Usage

### 1. Device Programming
- **Sensor Node**: sensor_node.ino via Arduino IDE
- **Gateway**: Load gateway.ino via Arduino IDE
- **Configure WiFi credentials** in both codes

### 2. Infrastructure
- **EMQX Broker**: Run locally on localhost:1883
- **Dashboard**: Open dashboard.html in modern web browser

### 3. Data Flow
1. PZEM-004T measures electrical parameters
2. Sensor node transmits data via LoRa
3. Gateway receives and forwards via MQTT
4. Dashboard visualizes in real-time
5. Data is captured and exported to CSV

## Architecture Advantages

- **Scalability**: Easy to add more sensor nodes
- **Range**: LoRa allows greater distance between sensor and gateway
- **Flexibility**: Gateway can connect to different WiFi networks
- **Analysis**: Multiple data points for comparative analysis

## Academic Information

**Author**: Jhon Jaime Vaca Hincapie  
**Program**: Master's in Automatic Engineering and Cyber-Physical Systems  
**Institution**: Fundación Universitaria Los Libertadores

## License

MIT License - Developed for academic and research purposes.

---

*Scalable IoT system for distributed electrical monitoring - Master's Degree Project*
