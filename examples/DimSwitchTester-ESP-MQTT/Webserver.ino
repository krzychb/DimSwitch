/*
  File: Webserver.ino, addition to DimSwitchTester-ESP-MQTT.ino
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


void handleRoot(void)
{
  String message = "hello from esp8266!";
  server.send(200, "text/plain", message);
}


void handleNotFound(void)
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


//
// web server set up and events
//
void setupWebserver(void)
{

  server.on("/", handleRoot);

  server.on("/dimswitch", []() {
    Serial.print("HTTP > ");

    for (uint8_t i = 0; i < server.args(); i++)
    {
      if (server.argName(i) == "Switch")
      {
        bool lampState = (server.arg(i) == "ON") ? HIGH : LOW;
        Serial.print(" Switch > ");
        lamp.power(lampState);
      }
      else if (server.argName(i) == "TargetIntensity")
      {
        Serial.print(" TargetIntensity > ");
        targetIntensity = server.arg(i).toInt();
        lamp.setIntensity(targetIntensity);
      }
      else if (server.argName(i) == "Calibrate")
      {
        Serial.print(" Calibrate > ");
        lamp.calibrate();
      }
      else if (server.argName(i) == "Quit")
      {
        Serial.print(" Quit > ");
        lamp.quit();
      }
      else
      {
        Serial.println("unknown argument! ");
        Serial.print(server.argName(i));
        Serial.print(": ");
        Serial.print(server.arg(i));
        Serial.print(" > ");
      }
    }
    Serial.println("done");

    showControlScreen();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}


void showControlScreen(void)
{
  String message;
  message += "<html><head>";
  message += "<title>DimSwith Library Tester</title>";
  message += "<meta http-equiv=\"refresh\" content=\"6; url=/dimswitch\">";
  message += "</head><body>";
  message += "<h3><a href=\"/dimswitch\">DimSwitch</a></h3>";
  message += "Light is: ";
  if (lamp.getState() == HIGH)
  {
    message += "ON, switch it <a href=\"/dimswitch?Switch=OFF\">OFF</a><br/>";
  }
  else
  {
    message += "OFF, switch it <a href=\"/dimswitch?Switch=ON\">ON</a><br/>";
  }
  message += "Light Intensity: " + (String) lamp.readIntensityPercent() + "%<br/>";
  message += "<form action=\"/dimswitch\" method=\"get\">";
  message += "Change to: <input type=\"text\" name=\"TargetIntensity\" size=\"3\" value=\"" + (String) targetIntensity + "\"><input type=\"submit\" value=\"Submit\">";
  message += "</form>";
  message += "Light sensor ADC<br/>";
  message += "Now: " + (String) lamp.readIntensityCounts() + "<br/>";
  message += "Min: " + (String) lamp.minIntensity + "<br/>";
  message += "Max: " + (String) lamp.maxIntensity + "<br/>";
  message += "<a href=\"/dimswitch?Calibrate=\">Calibrate</a><br/>";
  message += "</body>";
  message += "</html>";
  server.send(200, "text/html", message);
}


