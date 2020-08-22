#pragma once
#ifndef _BME280_H
#define _BME280_H

#include <Arduino.h>

void BME280Setup(void);
void BME280PrintValues(void);
float BME280ReadTemperature(void);
float BME280ReadHumidity(void);
float BME280ReadPressure(void);

#endif
