#pragma once
#ifndef _TSL2591_H
#define _TSL2591_H

#include <Arduino.h>

void TSL2591ShowGain(void);
bool TSL2591UpperGain(void);
bool TSL2591LowerGain(void);
void TSL2591AutoGain(void);
float TSL2591GetLux(bool, bool);
void TSL2591Setup(void);

#endif
