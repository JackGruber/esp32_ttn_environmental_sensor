#pragma once
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include <Arduino.h>

void PrintResetReason(void);
void SetupPins();
long ReadVBat();
void I2CScanner(void);
bool I2CCheckAddress(byte);

#endif
