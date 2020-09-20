#include <veml6075.h>
#include <Wire.h>
#include "Adafruit_VEML6075.h"

Adafruit_VEML6075 UVSENSOR = Adafruit_VEML6075();

void VEML6075Setup(void)
{
    Serial.println(F("Setup VEML6075 sensor"));
    if (!UVSENSOR.begin())
    {
        Serial.println("Failed to communicate with VEML6075 sensor!");
    }
    else
    {
        Serial.println("Found VEML6075 sensor");

        UVSENSOR.setIntegrationTime(VEML6075_100MS);
        Serial.print("Integration time set to ");
        switch (UVSENSOR.getIntegrationTime())
        {
        case VEML6075_50MS:
            Serial.print("50");
            break;
        case VEML6075_100MS:
            Serial.print("100");
            break;
        case VEML6075_200MS:
            Serial.print("200");
            break;
        case VEML6075_400MS:
            Serial.print("400");
            break;
        case VEML6075_800MS:
            Serial.print("800");
            break;
        }
        Serial.println(" ms");

        // Set the high dynamic mode
        UVSENSOR.setHighDynamic(true);
        if (UVSENSOR.getHighDynamic())
        {
            Serial.println("High dynamic reading mode");
        }
        else
        {
            Serial.println("Normal dynamic reading mode");
        }

        // Set the mode
        UVSENSOR.setForcedMode(false);
        if (UVSENSOR.getForcedMode())
        {
            Serial.println("Forced reading mode");
        }
        else
        {
            Serial.println("Continuous reading mode");
        }

        // Set the calibration coefficients
        // https://learn.adafruit.com/adafruit-veml6075-uva-uvb-uv-index-sensor/arduino-test
        // https://en.wikipedia.org/wiki/Ultraviolet_index
        UVSENSOR.setCoefficients(2.22, 1.33,          // UVA_A and UVA_B coefficients
                                 2.95, 1.74,          // UVB_C and UVB_D coefficients
                                 0.001461, 0.002591); // UVA and UVB responses
    }
}

float VEML6075GetUVI(void)
{
    UVSENSOR.shutdown(false);
    delay(200); // Wait for wakeup
    
    float uva = UVSENSOR.readUVA();
    float uvb = UVSENSOR.readUVB();
    float uvi = UVSENSOR.readUVI();
    
    UVSENSOR.shutdown(true);

    Serial.print("UVA: ");
    Serial.println(uva);
    Serial.print("UVB: ");
    Serial.println(uvb);
    Serial.print("UVI: ");
    Serial.println(uvi);
    
    return uvi;
}
