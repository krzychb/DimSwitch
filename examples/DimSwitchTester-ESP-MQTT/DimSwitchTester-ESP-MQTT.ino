/*
  File: DimSwitchTester-ESP-MQTT.ino
  DimSwitch library tester for Arduino platform

  Example implementation for ESP8266 borad
  to control dimmable ballasts for fluorescent light tubes
  https://github.com/krzychb/DimSwitch

  Note: For testing on Arduino UNO, MEGQ, DUE, Micro
  and similar Arduino compatible boards use TestDimSwitch.ino

  Copyright (c) 2016 Krzysztof Budzynski. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

//
// DimSwitch Configuration
//
#include "DimSwitch.h"
// Analog input pin to read light intensity
#define LIGHT_INTENSITY A0
// Digital output pin to control relay
#define LIGHT_SWITCH 5
DimSwitch lamp(LIGHT_INTENSITY, LIGHT_SWITCH);
// Target light intensity (0 - 100%)
int targetIntensity = 50;


//
// Wi-Fi Setup
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
const char* ssid = "********";     // your network SSID (name)
const char* password = "********";  // your network password
// function prototypes
void setupWiFi(void);

//
// HTTP Server Setup
//
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
void handleRoot(void);
void handleNotFound(void);
void showControlScreen(void);
void setupWebserver(void);


//
// OTA Configuration
//
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const char* host = "OTA-DimSwitch";
// function prototypes
void setupOTA(void);


//
// MQTT Configuration
//
// MQTT publish topics
#define LIGHT_INTENSITY_PERCENT "sensor/LightIntensity"
#define LAMP_STATE_SENSOR "sensor/LightSwitch"
#define SENSOR_COUNTS "sensor/LightSensor"
#define SENSOR_MIN "sensor/MinIntensity"
#define SENSOR_MAX "sensor/MaxIntensity"
// MQTT subscribe topics
#define LIGHT_INTENSITY_SETPOINT "control/LightIntensity"
#define LAMP_STATE_COMMAND "control/LightSwitch"
#define SENSOR_CALIBRATE "control/Calibrate"
//
#include <PubSubClient.h>
WiFiClient mqttClient;
PubSubClient MQTTclient(mqttClient, "test.mosquitto.org");
const char* MQTTclientName = "DimSwitch";
// function prototypes
void MQTTcallback(const MQTT::Publish& pub);
void MQTTpublishData(void);
void MQTTkeepAlive(void);


void setup(void)
{
  Serial.begin(115200);
  Serial.println("DimSwitchTester for ESP8266");
  setupWiFi();
  setupOTA();
  setupWebserver();

  MQTTclient.set_callback(MQTTcallback);
}


void loop(void)
{
  ArduinoOTA.handle();

  lamp.runSequence();
  server.handleClient();
  MQTTkeepAlive();
  if (millis() % 20000 == 0)
  {
    MQTTpublishData();
  }
}

