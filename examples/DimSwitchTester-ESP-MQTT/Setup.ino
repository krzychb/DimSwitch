/*
  File: Setup.ino, addition to DimSwitchTester-ESP-MQTT.ino
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


void setupWiFi(void)
{
  WiFi.begin(ssid, password);
  Serial.println();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


//
// OTA set up and events
//
void setupOTA(void)
{
  ArduinoOTA.setHostname(host);

  ArduinoOTA.onStart([]() {
    Serial.println("OTA upload start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("OTA upload end");
    Serial.println("Restarting...");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA initialized");
}

