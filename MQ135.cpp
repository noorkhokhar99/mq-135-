/*
  MQ135.cpp - Arduino library for MQ135 gas sensor
   2019
  MIT license
*/

#include "MQ135.h"

/*
  Constructor, initializes the MQ135 sensor connected to the specified analog pin.
*/
MQ135::MQ135(uint8_t pin) {
  _pin = pin;
  pinMode(_pin, INPUT);
  setEnvironmentalData(25, 50);
  resetCalibration();
}

/*
  Used for CO2 correction based on given temperature and humidity.
*/
void MQ135::setEnvironmentalData(float temperature, float humidity) {
  temperature = constrain(temperature, 0, 50);
  humidity = constrain(humidity, 0, 100);
  
  #if SENSOR_MANUFACTURER == 0
    _environmentalGain = (0.000063776 * humidity - 0.015774824) * temperature + (-0.007499500 * humidity + 1.782167832);
  #elif SENSOR_MANUFACTURER == 1
    if (temperature < 20) {
	  _environmentalGain = -0.019 * temperature + 1.38;
    } else {
      _environmentalGain = -0.003 * temperature + 1.06;
	}
	_environmentalGain = _environmentalGain * (-0.001923077 * humidity + 1.063461538);
  #else
    _environmentalGain = 1;
  #endif
}

/*
  Handles baseline calibration.
*/
void MQ135::handleCalibration(uint16_t &sample) {
  if (sample < _sampleBaseline) {
    _sampleBaseline = sample;
    _lastCalibration = millis();
  }
  
  if (sample < _sampleLowest) {
    _sampleLowest = sample;
  }
  
  if (millis() - _lastCalibration > CALIBRATION_PERIOD) {
    _sampleBaseline = _sampleLowest;
    _sampleLowest = 1023;
    _lastCalibration = millis();
  }
}

/*
  Resets baseline calibration.
*/
void MQ135::resetCalibration() {
  _sampleBaseline = 1023;
  _sampleLowest = 1023;
  _lastCalibration = millis();
}

/*
  Returns CO2 concentration in units of ppm.
*/
uint16_t MQ135::getCO2() {
  uint16_t sample = analogRead(_pin);
  handleCalibration(sample);
  float sampleGained = _sampleBaseline + _environmentalGain * (sample - _sampleBaseline);
  uint16_t co2 = (uint16_t) ATMOSPHERIC_CO2 * pow(_sampleBaseline / sampleGained, SENSOR_POWER);
  co2 = constrain(co2, ATMOSPHERIC_CO2, 65535);
  return co2;
}