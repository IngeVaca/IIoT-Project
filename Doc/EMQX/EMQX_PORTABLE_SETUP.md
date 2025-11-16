# EMQX Installation Guide - Portable Mode

## Download EMQX

### 1. Get Portable Version

- Visit official website: [https://www.emqx.io/downloads](https://www.emqx.io/downloads)
- Select **"Windows ZIP"** for portable version
- Download the latest stable release (e.g., `emqx-5.x.x-windows-amd64.zip`)

### 2. Extraction Process

- Create an `EMQX` folder in your desired location (e.g., `C:\EMQX\`)
- Extract all ZIP contents into this folder
- The resulting structure should be:

EMQX/

├── bin/ Executables and scripts

├── etc/ Configuration files

├── log/ System logs

├── data/ Persistent data

├── lib/ System libraries

└── releases/ Release files


## Service Configuration

### 3. Start EMQX Service
- Navigate to the `EMQX/bin/` folder
- Run **CMD as Administrator** in this location
- Execute the start command:

cmd
emqx start

### 4. Service Status Verification
In the same CMD window, verify the service is active:

emqx_ctl status

Should display: Node emqx@127.0.0.1 is started

## Initial Access and Setup

### 5. Web Dashboard Access
   
Open web browser

Navigate to: http://localhost:18083

Default credentials:

Username: admin

Password: public

### 6. Security Enhancement (Recommended)

In the dashboard, go to "Administration" → "Users"

Select admin user

Click "Change Password"

Set a new secure password

6. Security Enhancement (Recommended)
In the dashboard, go to "Administration" → "Users"

Select admin user

Click "Change Password"

Set a new secure password

## Port Configuration

### 7. Service Ports Verification
The following ports should be active:

1883: MQTT TCP (for ESP32 devices)

8083: MQTT WebSocket (for web dashboard)

18083: Administrative dashboard

##Management Commands

Start Service: 

emqx start

Stop Service

emqx stop

Restart Service

emqx restart

Check Status

emqx_ctl status
