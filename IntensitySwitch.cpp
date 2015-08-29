#include "Arduino.h"
#include "IntensitySwitch.h"

IntensitySwitch::IntensitySwitch(int intensityMeasurePin, int switchPin)
{
  _intensityMeasurePin = intensityMeasurePin;
  _switchPin = switchPin;
  pinMode(_intensityMeasurePin, INPUT);
  pinMode(_switchPin, OUTPUT);
}


void IntensitySwitch::runSequence()
{
  // ====================
  // Toggle Switch
  // ====================
  if (stateName == TOGGLE_ON)
  {
    _sequenceTimer = millis();
    Serial.print("\tIdle > ");
    digitalWrite(_switchPin, HIGH);
    _stateTimer = millis();
    stateName = TOGGLE_ON__FINALIZE;
    Serial.print("\Toggle On > ");
  }
  else if (stateName == TOGGLE_ON__FINALIZE)
  {
    if (millis() > _stateTimer + TOGGLE_ON_DURATION)
    {
      digitalWrite(_switchPin, LOW);
      stateName = SEQUENCE_IDLE;
      Serial.println("Idle");
    }
  }
  // ====================
  // Power On
  // ====================
  else if (stateName == POWER_ON)
  {
    _sequenceTimer = millis();
    Serial.print("\tIdle > ");
    if (readIntensityCounts() <= MIN_INTENSITY/2)
    {
      digitalWrite(_switchPin, HIGH);
      _stateTimer = millis();
      stateName = POWER_ON__TOGGLE_ON;
      Serial.print("Toggle On > ");
    }
    else
    {
      stateName = SEQUENCE_IDLE;
      Serial.println("Already On! > Idle");
    }
  }
  else if (stateName == POWER_ON__TOGGLE_ON)
  {
    if (millis() > _stateTimer + TOGGLE_ON_DURATION)
    {
      digitalWrite(_switchPin, LOW);
      _stateTimer = millis();
      stateName = POWER_ON__IS_ON;
      Serial.print("Is On? > ");
    }
  }
  else if (stateName == POWER_ON__IS_ON)
  {
    if (millis() > _stateTimer + POWER_ON_DURATION)
    {
      if (readIntensityCounts() <= MIN_INTENSITY/2)
      {
        Serial.print("Not On! > ");
      }
      stateName = SEQUENCE_IDLE;
      Serial.println("Idle");
    }
  }
  // ====================
  // Power Off
  // ====================
  else if (stateName == POWER_OFF)
  {
    _sequenceTimer = millis();
    Serial.print("\tIdle > ");
    if (readIntensityCounts() > MIN_INTENSITY/2)
    {
      digitalWrite(_switchPin, HIGH);
      _stateTimer = millis();
      stateName = POWER_OFF__TOGGLE_ON;
      Serial.print("Toggle On > ");
    }
    else
    {
      stateName = SEQUENCE_IDLE;
      Serial.println("Already Off! > Idle");
    }
  }
  else if (stateName == POWER_OFF__TOGGLE_ON)
  {
    if (millis() > _stateTimer + TOGGLE_ON_DURATION)
    {
      digitalWrite(_switchPin, LOW);
      _stateTimer = millis();
      stateName = POWER_OFF__IS_OFF;
      Serial.print("Is Off? > ");
    }
  }
  else if (stateName == POWER_OFF__IS_OFF)
  {
    if (millis() > _stateTimer + POWER_OFF_DURATION)
    {
      if (readIntensityCounts() > MIN_INTENSITY/2)
      {
        Serial.print(" Not Off! > ");
      }
      stateName = SEQUENCE_IDLE;
      Serial.println("Idle");
    }
  }
  // ====================
  // Calibrate
  // ====================
  else if (stateName == CALIBRATE)
  {
    _sequenceTimer = millis();
    _intensityChangeTimer = _sequenceTimer;
    _lastIntensity = readIntensityCounts();
    Serial.print("\tIdle > ");
    digitalWrite(_switchPin, HIGH);
    _stateTimer = millis();
    stateName = CALIBRATE__ACCELERATE;
    Serial.print("Accelerate > ");
  }
  else if (stateName == CALIBRATE__ACCELERATE)
  {
    if (millis() > _stateTimer + ACCELERATE_DURATION)
    {
      stateName = CALIBRATE__TO_LIMIT;
      Serial.print("To Limit > ");
    }
  }
  else if (stateName == CALIBRATE__TO_LIMIT)
  {
    // move to next step when time limit is reached or intensity does not change
    if (millis() > _stateTimer + TRAVEL_MAX_DURATION || abs(_intensityChange) <= DB_INTENSITY_CHANGE)
    {
      Serial.println();
      Serial.print("\tGet to ");
      Serial.print(readIntensityCounts());
      Serial.print(" > ");
      digitalWrite(_switchPin, LOW);
      _stateTimer = millis();
      stateName = CALIBRATE__TOGGLE_OFF;
      Serial.print("Toggle Off > ");
    }
  }
  else if (stateName == CALIBRATE__TOGGLE_OFF)
  {
    if (millis() > _stateTimer + TOGGLE_OFF_DURATION)
    {
      _sequenceTimer = millis();
      digitalWrite(_switchPin, HIGH);
      _stateTimer = millis();
      stateName = CALIBRATE__ACCELERATE_OTHER;
      Serial.print("Accelerate > ");
    }
  }
  else if (stateName == CALIBRATE__ACCELERATE_OTHER)
  {
    if (millis() > _stateTimer + ACCELERATE_DURATION)
    {
      stateName = CALIBRATE__TO_OTHER_LIMIT;
      Serial.print("To Other Limit > ");
    }
  }
  else if (stateName == CALIBRATE__TO_OTHER_LIMIT)
  {
    // finish when time limit is reached or intensity does not change
    if (millis() > _stateTimer + TRAVEL_MAX_DURATION || abs(_intensityChange) <= DB_INTENSITY_CHANGE)
    {
      digitalWrite(_switchPin, LOW);
      Serial.println();
      Serial.print("\tGet to ");
      Serial.print(readIntensityCounts());
      Serial.print(" > ");
      stateName = SEQUENCE_IDLE;
      Serial.println("Idle");
    }
  }
  // ====================
  // Set Intensity
  // ====================
  else if (stateName == SET_INTENSITY)
  {
    _sequenceTimer = millis();
    _intensityChangeTimer = _sequenceTimer;
    _lastIntensity = readIntensityCounts();
    Serial.print("\tIdle > ");
    if (abs(_targetIntensity - readIntensityCounts()) < DB_INTENSITY)
    {
      Serial.print("On Target! > ");
      stateName = SEQUENCE_IDLE;
      Serial.println("Idle");
    }
    else
    {
      int currentIntensity = readIntensityCounts();
      digitalWrite(_switchPin, HIGH);
      _stateTimer = millis();
      if (currentIntensity > MIN_INTENSITY/2)
      {
        stateName = SET_INTENSITY__ACCELERATE;
      }
      else
      {
        stateName = SET_INTENSITY__GET_ON;
        Serial.print("Get On > ");
      }
      Serial.print("Moving from ");
      Serial.print(currentIntensity);
      Serial.print(" to ");
      Serial.print(_targetIntensity);
      Serial.print(" > ");
    }
  }
  else if (stateName == SET_INTENSITY__GET_ON)
  {
    if (millis() > _stateTimer + POWER_ON_DURATION - ACCELERATE_DURATION)
    {
      _stateTimer = millis();
      stateName = SET_INTENSITY__ACCELERATE;
    }
  }
  else if (stateName == SET_INTENSITY__ACCELERATE)
  {
    if (millis() > _stateTimer + ACCELERATE_DURATION)
    {
      int currentIntensity = readIntensityCounts();
      boolean goingUp = (_intensityChange > 0) ? true : false;
      _targetAbove = (currentIntensity < _targetIntensity) ? true : false;
      Serial.println();
      Serial.print("\tMoved by ");
      Serial.print(_intensityChange);
      Serial.print(" to ");
      Serial.print(currentIntensity);
      Serial.print(", ");
      if (_targetAbove && goingUp || !_targetAbove && !goingUp)
      {
        stateName = SET_INTENSITY__GET_TARGET;
        Serial.print("Get Target > ");
      }
      else
      {
        digitalWrite(_switchPin, LOW);
        _stateTimer = millis();
        stateName = SET_INTENSITY__TOGGLE_OFF;
        Serial.print("Toggle Off > ");
      }
    }
  }
  else if (stateName == SET_INTENSITY__TOGGLE_OFF)
  {
    if (millis() > _stateTimer + TOGGLE_OFF_DURATION)
    {
      digitalWrite(_switchPin, HIGH);
      _stateTimer = millis();
      stateName = SET_INTENSITY__ACCELERATE_OTHER;
      Serial.print("Accelerate > ");
    }
  }
  else if (stateName == SET_INTENSITY__ACCELERATE_OTHER)
  {
    if (millis() > _stateTimer + ACCELERATE_DURATION)
    {
      stateName = SET_INTENSITY__GET_TARGET;
      Serial.print("Get Target > ");
    }
  }
  else if (stateName == SET_INTENSITY__GET_TARGET)
  {
    boolean valueAbove = (readIntensityCounts() > _targetIntensity) ? true : false;
    // finalize when target is reached or intensity does not change
    if (_targetAbove && valueAbove || !_targetAbove && !valueAbove || abs(_intensityChange) <= DB_INTENSITY_CHANGE)
    {
      stateName = SET_INTENSITY__FINALIZE;
    }
  }
  else if (stateName == SET_INTENSITY__FINALIZE)
  {
    digitalWrite(_switchPin, LOW);
    stateName = SEQUENCE_IDLE;
    Serial.println();
    Serial.print("\tFinalized at ");
    Serial.print(readIntensityCounts());
    Serial.print(" > ");

    Serial.println("Idle");
  }
  // ========================================
  // Common Tasks & Check for Timeout
  // ========================================
  if (stateName != SEQUENCE_IDLE)
  {
    if (millis() > _sequenceTimer + SEQUENCE_TIMEOUT)
    {
      stateName = SEQUENCE_IDLE;
      Serial.println("Timeout! > Idle");
    }
    if ((stateName & SET_INTENSITY) == SET_INTENSITY || (stateName & CALIBRATE) == CALIBRATE)
    {
      if (millis() > _intensityChangeTimer + SENSE_CHANGE_DURATION)
      {
        int currentIntensity = readIntensityCounts();
        _intensityChange = currentIntensity - _lastIntensity;
        _lastIntensity = currentIntensity;
        _intensityChangeTimer = millis();
      }
    }
  }
}


int IntensitySwitch::readIntensityCounts()
{
  int valueAcc = 0;
  for (int i = 0; i < MEASURE_SAMPLES; i++)
    valueAcc += analogRead(_intensityMeasurePin);
  return (int) valueAcc / MEASURE_SAMPLES;
}


int IntensitySwitch::readIntensityPercent()
{
  return (int) (100.0 * (readIntensityCounts() - MIN_INTENSITY) / (MAX_INTENSITY - MIN_INTENSITY));
}


void IntensitySwitch::toggleSwitch()
{
  stateName = TOGGLE_ON;
}


void IntensitySwitch::power(boolean state)
{
  if (state == HIGH)
    stateName = POWER_ON;
  else
    stateName = POWER_OFF;
}


void IntensitySwitch::calibrate()
{
  stateName = CALIBRATE;
}


void IntensitySwitch::setIntensity(int targetIntensityPercent)
{
  _targetIntensity = (int) (MIN_INTENSITY + (MAX_INTENSITY - MIN_INTENSITY) * (float) targetIntensityPercent / 100);
  stateName = SET_INTENSITY;
}


void IntensitySwitch::quit()
{
  if (digitalRead(_switchPin) == HIGH)
    digitalWrite(_switchPin, LOW);
  stateName = SEQUENCE_IDLE;
  Serial.println("Quit! > Idle");
}

