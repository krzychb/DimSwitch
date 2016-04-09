/*
  File: DimSwitchTester.ino
  DimSwitch library tester for Arduino platform
  
  Example implementation for Arduino UNO, MEGA, DUE, Micro 
  and similar Arduino compatible boards
  to control dimmable ballasts for fluorescent light tubes  
  https://github.com/krzychb/DimSwitch

  Note: For testing on ESP8266 borad use TestDimSwitch-ESP.ino

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
#define LIGHT_SWITCH 4
DimSwitch lamp(LIGHT_INTENSITY, LIGHT_SWITCH);


void setup(void)
{
  randomSeed(analogRead(0));
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("DimSwitchTester");
  Serial.println("Type a letter + <enter>:");
  Serial.println("i - Show info");
  Serial.println("t - Toggle switch On");
  Serial.println("1 or 0 - Power On or Off");
  Serial.println("s - Set random intensity");
  Serial.println("c - Calibrate");
  Serial.println("q - Quit sequence in progress");
}


void loop(void)
{
  lamp.runSequence();

  if (Serial.available() > 0)
  {
    char inChar = Serial.read();
    switch (inChar)
    {
      case 'i':
        Serial.print("Current intensity ");
        Serial.print(lamp.readIntensityPercent());
        Serial.print("% / ");
        Serial.println(lamp.readIntensityCounts());
        Serial.print("Min / Max intensity ");
        Serial.print(lamp.minIntensity);
        Serial.print(" / ");
        Serial.println(lamp.maxIntensity);
        break;
      case 't':
        Serial.println("Toggle switch On");
        lamp.toggle();
        break;
      case '1':
        Serial.println("Power On");
        lamp.power(HIGH);
        break;
      case '0':
        Serial.println("Power Off");
        lamp.power(LOW);
        break;
      case 's':
        int lampIntensityPercent;
        lampIntensityPercent = random(0, 101);
        Serial.print("Set Intensity from ");
        Serial.print(lamp.readIntensityPercent());
        Serial.print("% to ");
        Serial.print(lampIntensityPercent);
        Serial.println("%");
        lamp.setIntensity(lampIntensityPercent);
        break;
      case 'c':
        Serial.println("Calibrate");
        lamp.calibrate();
        break;
      case 'q':
        lamp.quit();
        break;
      default:
        break;
    }
  }
}
