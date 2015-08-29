#include "IntensitySwitch.h"

#define LAMP_INTENSITY A2
#define LAMP_SWITCH 4

IntensitySwitch lamp(LAMP_INTENSITY, LAMP_SWITCH);


void setup()
{
  Serial.begin(57600);
  Serial.println("Lamp Intensity Switch Tester");
  Serial.print("RAM ");
  Serial.print(freeRam());
  Serial.println(" bytes");

  randomSeed(analogRead(0));
}


void loop()
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
        break;
      case 't':
        Serial.println("Toggle switch On");
        lamp.toggleSwitch();
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


int freeRam(void)
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

