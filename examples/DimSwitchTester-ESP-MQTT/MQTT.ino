/*
  File: MQTT.ino, addition to DimSwitchTester-ESP-MQTT.ino
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


void MQTTcallback(const MQTT::Publish& pub)
{
  Serial.print("MQTT CALLBACK > ");

  if (pub.topic() == LAMP_STATE_COMMAND)
  {
    bool lampState = (pub.payload_string() == "ON") ? HIGH : LOW;
    Serial.print(" Switch > ");
    lamp.power(lampState);
  }
  else if (pub.topic() == LIGHT_INTENSITY_SETPOINT)
  {
    Serial.print(" TargetIntensity > ");
    targetIntensity = pub.payload_string().toInt();
    lamp.setIntensity(targetIntensity);
  }
  else if (pub.topic() == SENSOR_CALIBRATE)
  {
    if (pub.payload_string() == "ON")
    {
      Serial.print(" Calibrate > ");
      lamp.calibrate();
    }
    else
    {
      Serial.println(" Unknown Command!");
    }
  }
  else
  {
    Serial.print("Unknown! > ");
    Serial.print(pub.topic());
    Serial.print(" ");
    Serial.println(pub.payload_string());
  }
}


void MQTTpublishData(void)
{
  Serial.print("MQTT PUBLISH > ");
  if (MQTTclient.connected())
  {
    // Publish values only if lamp sequence is idle
    if (lamp.isIdle() == true)
    {
      bool lampState = lamp.getState();
      // Publish state of the lamp
      MQTTclient.publish(LAMP_STATE_SENSOR, (lampState == HIGH) ? "ON" : "OFF");
      // Publish raw light sensor value
      MQTTclient.publish(SENSOR_COUNTS, (String) lamp.readIntensityCounts());
      // Publish min and max sensor calibration values
      MQTTclient.publish(SENSOR_MIN, (String) lamp.minIntensity);
      MQTTclient.publish(SENSOR_MAX, (String) lamp.maxIntensity);
      // Publish light intensity, but only if lamp is On
      // This is for slider tracking
      // that should remain in last state if light is turned off
      if (lampState == HIGH)
      {
        MQTTclient.publish(LIGHT_INTENSITY_PERCENT, (String) lamp.readIntensityPercent());
      }
      Serial.println("done");
    }
    else
    {
      Serial.println("aborted!");
    }
  }
  else
  {
    Serial.println("not connected!");
  }
}


void MQTTkeepAlive(void)
{
  if (!MQTTclient.connected())
  {
    Serial.print("MQTT CONNECT and SUBSCRIBE > ");
    MQTTclient.connect(MQTTclientName);
    // Subscribe to command to switch the lamp On / Off
    MQTTclient.subscribe(LAMP_STATE_COMMAND);
    // Subscribe to command to set light intensity to specific value
    MQTTclient.subscribe(LIGHT_INTENSITY_SETPOINT);
    // Subscribe to lamp calibration command
    MQTTclient.subscribe(SENSOR_CALIBRATE);
    Serial.println("done");
  }
  else
  {
    MQTTclient.loop();
  }
}

