/**************************************************************************
 This is an example for Temperatute and Humidity Iot ESP32 micro 

 Git repository:
 ------> https://github.com/curiousmindos/iot-temperature-humidity-esp32/blob/main/iot-temperature-sensors-esp32.ino

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset)
 and IoT sensor (temperature and humidity)

 Written by Oleg Semyonov for SmartLegoHome,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the code below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 2       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11

DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "your wifi ssid";
const char* password = "your password";
String MQTT_BROKER = "http://192.168.1.10:80";
String serverBrokerPath = "/update-sensors/sensors/";
String urlTemperaturePath = "temperature/update/v1";

String _uuid;
int smallSendCounter = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("Temperature Sensors IoT - Temperature and Humidity:");
  dht.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);  // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // read ESP model name
  _uuid = ESP.getChipModel();
  
  // welcome screen
  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setTextColor(WHITE, BLACK);
  display.println("  -= IoT Sensors =-");
  display.println("Temperature/Humidity.");
  display.println("Model: " + _uuid);
  display.setCursor(30, 25);
  display.setTextColor(BLACK, WHITE);
  display.println(" ver 1.7 ");

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  // WiFi status
  display.clearDisplay(); 
  display.setCursor(0, 0);
  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setTextColor(WHITE, BLACK); 
  display.println("WiFi connecting");
  display.display();

  WiFi.begin(ssid, password);
  Serial.println("Connecting");   
  
  // WiFi connection progress
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
     display.print(".");
     display.display();
  } 
  
  // WiFi connected and client IP address
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay(); 
  display.setCursor(0, 10);
  display.println("WiFi connected.\nIP:" + WiFi.localIP().toString());  
  display.display();  
  
  _uuid +="#" + WiFi.macAddress();

  delay(2000);
}

void loop() {

  delay(2000);

  // read sensors values
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  bool isError = isnan(h) || isnan(t);

  display.clearDisplay();
  // display it on OLED display
  sensorValuesDisplay(t, h, isError);
  // push values into Http server
  sendToServer(t, h, isError);

  display.display();
}

void sensorValuesDisplay(float t, float h, bool isError) {

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %	");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" °C");  

  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);  // Start at top-left corner
  display.cp437(true);      // Use full 256 char 'Code Page 437' font

  if (isError) {
    display.write("Failed to read from DHT sensor!");
  } else {
    int f = (t * 1.8) + 32;
    display.println("Temp (C/F)    Humid %");
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 9);
    display.setTextSize(2);
    display.println(String((int)t) + "(" + String(f) + ") " + String((int)h));
    display.setTextSize(1);
    display.print("server: ");
    if (WiFi.status() == WL_CONNECTED){
      display.print("connected ");      
    }
    else {
      display.setTextColor(BLACK, WHITE);
      display.print("not connected ");
    }
  }  
}

void sendToServer(float t, float h, bool isError) {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String serverPath = MQTT_BROKER + serverBrokerPath + urlTemperaturePath + "?deviceName=" + _uuid + "&temperature=" + t + "&humidity=" + h;

    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());

    // If you need Node-RED/server authentication, insert user and password below
    //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      
      if(smallSendCounter++ > 10)
        smallSendCounter = 1; 

      // display 10 send counters attempts
      for(int at = 0; at < smallSendCounter; at++){
        for(int p = 1; p < 6; p++){
         display.drawPixel(SCREEN_WIDTH - p, SCREEN_HEIGHT - at*2 - 1, SSD1306_WHITE);
        }
      }

    } else { 
      display.setCursor(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 8);
      display.setTextColor(BLACK, WHITE);
      display.print(" err");
      
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

