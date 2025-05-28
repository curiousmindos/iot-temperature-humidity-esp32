# ESP32 Sensor Monitoring System
ESP32 Sensor Monitoring System

A compact and Wi-Fi-enabled real-time sensor monitoring system powered by the ESP32 microcontroller. This project reads data from connected sensors (temperature, humidity) and publishes it via a web server, MQTT.

## Features

Wi-Fi connectivity using ESP32

🌡️ Real-time sensor data acquisition (e.g., DHT11/DHT22, MQ-2, HC-SR04, LDR, PIR)

🌍 Local web server dashboard or MQTT publishing

📊 In progress: Optional integration with cloud platforms (ThingSpeak, Blynk, Home Assistant, etc.)

🔔 In progress: Email or mobile alert support (e.g., when motion is detected or thresholds exceeded)

💤 Low power/sleep mode support for battery efficiency

## Hardware Used
ESP32 Dev Module

DHT11 / DHT22 – Temperature & humidity sensor

📦 Installation & Setup

Clone the repository:

git clone 
https://github.com/curiousmindos/iot-temperature-humidity-esp32.git

cd iot-temperature-humidity-esp32

Open the project in Arduino IDE or PlatformIO.

Install the required libraries:

DHT sensor library

Adafruit Unified Sensor

WiFi.h (built-in)


Configure the config.h or secrets.h file:

const char* ssid "your_wifi"
const char* password "your_password"
String MQTT_BROKER "192.168.1.100"

Upload the code to your ESP32 and monitor the serial output.

📷 Screenshots
![image](https://github.com/user-attachments/assets/58c30c47-44c7-4e9b-9a44-7e2d39ac7369)

![image](https://github.com/user-attachments/assets/f7bdcb38-4b7b-4716-98ce-ff6da8d7c1cf)

💡 Use Cases

Home environment monitoring

Smart farming (e.g., soil moisture + temperature)

Educational/learning IoT projects

📜 License
This project is licensed under the MIT License.
