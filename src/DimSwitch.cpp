#include "Arduino.h"
#include "DimSwitch.h"

//
// Object constructor
//
// intensityMeasurePin - input pin where light sensor is connected to
// switchPin - output pin control relay is connected to
//
DimSwitch::DimSwitch(int intensityMeasurePin, int switchPin)
{
  _intensityMeasurePin = intensityMeasurePin;
  _switchPin = switchPin;
  pinMode(_intensityMeasurePin, INPUT);
  pinMode(_switchPin, OUTPUT);
  stateName = SEQUENCE_IDLE;
  //
  // ToDo:
  // Retrieve maxIntensity and minIntensity from flash / EPROM
  // calculate dbIntensity
  //
}


//
// State machine sequence that operates the control relay
//
void DimSwitch::runSequence(void)
{
  // ====================
  // Do Nothing
  // ====================
  if (stateName == SEQUENCE_IDLE)
  {
	  return;
  }

  // ====================
  // Exit if too soon
  // ====================
  if (millis() < _lastCalled + SEQUENCE_PERIOD)
  {
	  return;
  }

  _lastCalled = millis();
  _currentIntensity = readIntensityCounts();

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
    Serial.print("Toggle On > ");
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
    if (_currentIntensity <= minIntensity/2)
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
      if (_currentIntensity <= minIntensity/2)
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
    if (_currentIntensity > minIntensity/2)
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
      if (_currentIntensity > minIntensity/2)
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
    _lastIntensity = _currentIntensity;
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
      Serial.print("\tGot to ");
      minIntensity = _currentIntensity;
      Serial.print(minIntensity);
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
      Serial.print("\tGot to ");
      maxIntensity = _currentIntensity;
      Serial.print(maxIntensity);
      //
      if(maxIntensity < minIntensity)
      {
		int tmpVal = maxIntensity;
		maxIntensity = minIntensity;
		minIntensity = tmpVal;
      }
      //
      // ToDo:
      // Save maxIntensity and minIntensity to flash / EPROM
      //
      dbIntensity = (maxIntensity - minIntensity) / DB_RANGE_DIVIDER;
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
    _lastIntensity = _currentIntensity;
    Serial.print("\tIdle > ");
    if (abs(_targetIntensity - _currentIntensity) < dbIntensity)
    {
      Serial.print("On Target! > ");
      stateName = SEQUENCE_IDLE;
      Serial.println("Idle");
    }
    else
    {
      digitalWrite(_switchPin, HIGH);
      _stateTimer = millis();
      if (_currentIntensity > minIntensity/2)
      {
        stateName = SET_INTENSITY__ACCELERATE;
      }
      else
      {
        stateName = SET_INTENSITY__GET_ON;
        Serial.print("Get On > ");
      }
      Serial.print("Moving from ");
      Serial.print(_currentIntensity);
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
      bool goingUp = (_intensityChange > 0) ? true : false;
      _targetAbove = (_currentIntensity < _targetIntensity) ? true : false;
      Serial.println();
      Serial.print("\tMoved by ");
      Serial.print(_intensityChange);
      Serial.print(" to ");
      Serial.print(_currentIntensity);
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
    boolean valueAbove = (_currentIntensity > _targetIntensity) ? true : false;
    // finalise when target is reached or intensity does not change
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
    Serial.print(_currentIntensity);
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
	// Periodically sense light intensity change as required by certain states
    if ((stateName & SET_INTENSITY) == SET_INTENSITY || (stateName & CALIBRATE) == CALIBRATE)
    {
      if (millis() > _intensityChangeTimer + SENSE_CHANGE_DURATION)
      {
        int currentIntensity = _currentIntensity;
        _intensityChange = currentIntensity - _lastIntensity;
        _lastIntensity = currentIntensity;
        _intensityChangeTimer = millis();
      }
    }
  }
}


//
// Read light intensity
//
// Note: returned value is a raw output of ADC where the light sensor is connected to
//
int DimSwitch::readIntensityCounts(void)
{
  int valueAcc = 0;
  for (int i = 0; i < MEASURE_SAMPLES; i++)
  {
    valueAcc += analogRead(_intensityMeasurePin);
  }
  return (int) valueAcc / MEASURE_SAMPLES;
}

//
// Read light intensity
//
// Note: returned value is a percentage of current light intensity
// in relation of minimum and maximum light intensity values
//
int DimSwitch::readIntensityPercent(void)
{
  return (int) (100.0 * (readIntensityCounts() - minIntensity) / (maxIntensity - minIntensity));
}


//
// Get state of the lamp
//
// HIGH - the light is on, LOW - the light is off
//
bool DimSwitch::getState(void)
{
  return (readIntensityCounts() > minIntensity/2) ? HIGH : LOW;
}


//
// Check if sequence is idle
//
// HIGH - sequence is idle, LOW - sequence is running
//
bool DimSwitch::isIdle(void)
{
  return (stateName == SEQUENCE_IDLE) ? HIGH : LOW;
}


//
// Toggle output relay on
//
void DimSwitch::toggle(void)
{
  stateName = TOGGLE_ON;
}


//
// Initiate a sequence to power the light on on off
//
// state - to power the lamp on set it to logic true, to power off set it to false
//
void DimSwitch::power(bool state)
{
  stateName = (state == HIGH) ? POWER_ON : POWER_OFF;
}


//
// Initiate a sequence to calibrate readings of light intensity ADC
//
void DimSwitch::calibrate(void)
{
  stateName = CALIBRATE;
}


//
// Initiate a sequence to set specific light intensity
//
// targetIntensityPercent - light intensity to set
//
void DimSwitch::setIntensity(int targetIntensityPercent)
{
  _targetIntensity = (int) (minIntensity + (maxIntensity - minIntensity) * (float) targetIntensityPercent / 100);
  stateName = SET_INTENSITY;
}


//
// Quit the running sequence in progress
//
// Note: the control relay will be switched off
//
void DimSwitch::quit(void)
{
  if (digitalRead(_switchPin) == HIGH)
  {
    digitalWrite(_switchPin, LOW);
  }
  stateName = SEQUENCE_IDLE;
  Serial.println("Quit! > Idle");
}

