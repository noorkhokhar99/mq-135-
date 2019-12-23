/*
  MQ135.h - Arduino library for MQ135 gas sensor
  Created by Noor khokhar, 2019
  MIT license
*/

#ifndef MQ135_h
#define MQ135_h

#include "Arduino.h"

#define ATMOSPHERIC_CO2 400 // 400 ppm
#define CALIBRATION_PERIOD 604800000 // 7 days
#define SENSOR_MANUFACTURER 0 // 0 = Winsen, 1 = Olimex

#if SENSOR_MANUFACTURER == 0
  #define SENSOR_POWER -2.157884928
#elif SENSOR_MANUFACTURER == 1
  #define SENSOR_POWER -2.821874926
#else
  #define SENSOR_POWER -2.0
#endif

class MQ135 {
  public:
    MQ135(uint8_t pin);
    void setEnvironmentalData(float temperature, float humidity);
    void resetCalibration();
    uint16_t getCO2();

  private:
    uint8_t _pin;
    float _environmentalGain;
    uint16_t _sampleLowest;
    uint16_t _sampleBaseline;
    uint32_t _lastCalibration;
    void handleCalibration(uint16_t &sample);
};

#endif