#include "tsl2591.h"
#include <Wire.h>
#include <Adafruit_TSL2591.h>

Adafruit_TSL2591 TSL2591 = Adafruit_TSL2591(2591);

void TSL2591ShowGain()
{
    tsl2591Gain_t gain = TSL2591.getGain();
    switch (gain)
    {
    case TSL2591_GAIN_LOW:
        Serial.print(F("1x (Low)"));
        break;
    case TSL2591_GAIN_MED:
        Serial.print(F("25x (Medium)"));
        break;
    case TSL2591_GAIN_HIGH:
        Serial.print(F("428x (High)"));
        break;
    case TSL2591_GAIN_MAX:
        Serial.print(F("9876x (Max)"));
        break;
    }
}

bool TSL2591UpperGain()
{
    tsl2591Gain_t gain = TSL2591.getGain();
    bool update = false;
    switch (gain)
    {
    case TSL2591_GAIN_LOW:
        TSL2591.setGain(TSL2591_GAIN_MED);
        update = true;
        break;
    case TSL2591_GAIN_MED:
        TSL2591.setGain(TSL2591_GAIN_HIGH);
        update = true;
        break;
    case TSL2591_GAIN_HIGH:
        TSL2591.setGain(TSL2591_GAIN_MAX);
        update = true;
        break;
    case TSL2591_GAIN_MAX:
        update = false;
        break;
    }

    return update;
}

bool TSL2591LowerGain()
{
    tsl2591Gain_t gain = TSL2591.getGain();
    bool update = false;
    switch (gain)
    {
    case TSL2591_GAIN_LOW:
        update = false;
        break;
    case TSL2591_GAIN_MED:
        TSL2591.setGain(TSL2591_GAIN_LOW);
        update = true;
        break;
    case TSL2591_GAIN_HIGH:
        TSL2591.setGain(TSL2591_GAIN_MED);
        update = true;
        break;
    case TSL2591_GAIN_MAX:
        TSL2591.setGain(TSL2591_GAIN_HIGH);
        update = true;
        break;
    }

    return update;
}

void TSL2591AutoGain()
{
    TSL2591.setGain(TSL2591_GAIN_LOW);
    float lux;
    bool gain_ok = false;

    while (gain_ok == false)
    {
        lux = TSL2591GetLux(false, false);

        if (lux > 0)
        {
            // Max gain and reading OK
            if (TSL2591UpperGain() == false && TSL2591GetLux(false, true) > 0)
            {
                gain_ok = true;
            }
        }
        else
        {
            TSL2591LowerGain();
            gain_ok = true;
        }
    }
}

float TSL2591GetLux(bool autogain = false, bool print = true)
{
    uint32_t lum;
    uint16_t ir, full;

    if (autogain == true)
    {
        TSL2591AutoGain();
    }

    lum = TSL2591.getFullLuminosity();
    ir = lum >> 16;
    full = lum & 0xFFFF;
    float lux = TSL2591.calculateLux(full, ir);

    if (print == true)
    {
        Serial.print(F("IR: "));
        Serial.print(ir);
        Serial.print(F("  "));
        Serial.print(F("Full: "));
        Serial.print(full);
        Serial.print(F("  "));
        Serial.print(F("Visible: "));
        Serial.print(full - ir);
        Serial.print(F("  "));
        Serial.print(F("Lux: "));
        Serial.print(lux, 6);
        Serial.print(F("  "));
        Serial.print(F("Auto Gain: "));
        if (autogain == true)
        {
            Serial.print(F("yes - "));
        }
        else
        {
            Serial.print(F("no - "));
        }
        TSL2591ShowGain();
        Serial.println(F("  "));
    }

    return lux;
}

void TSL2591Setup()
{
    Serial.println(F("Setup TSL2591 sensor"));

    if (TSL2591.begin())
    {
        Serial.println(F("Found a TSL2591 sensor"));

        TSL2591.setGain(TSL2591_GAIN_MED);
        TSL2591.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    }
}
