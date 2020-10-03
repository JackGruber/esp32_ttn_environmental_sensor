#include <Arduino.h>
#include <particle.h>
#include <lorawan.h>
#include <functions.h>
#include <global.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "version.h"
#include "version_build.h"
#include "bme280.h"
#include "esp32-hal-cpu.h"
#include <veml6075.h>
#include <power.h>
#include "settings.h"
#include <tsl2591.h>

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable Brownout Detector

    Serial.begin(115200);
    Serial.println(F("Starting environment sensor ..."));
    Serial.println("Sketch: " VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "." BUILD_COMMIT "-" BUILD_BRANCH);
    Serial.println("Builddate: " BUILD_DATE " " BUILD_TIME);
    LoraWANPrintVersion();
    PrintResetReason();

    setCpuFrequencyMhz(10);
    Serial.print("CPU Speed: ");
    Serial.print(getCpuFrequencyMhz());
    Serial.println(" MHz");

    SetupPins();

    Wire.begin();
    I2CScanner();

    if( ReadVBat() < 3300)
    {
        Serial.println("Goto DeepSleep (VBat to low)");
        PowerDeepSleepTimer(LORA_TX_INTERVAL);
    }

    // Setup BME280 and print values
    if(I2CCheckAddress(0x76))
    {
        BME280Setup();
        BME280PrintValues();
    }
    else
    {
        Serial.println("No BME280 found!");
    }

    // Setup VEML6075
    if(I2CCheckAddress(0x10))
    {
        VEML6075Setup();
        VEML6075GetUVI();
    }
    else
    {
        Serial.println("No VEML6075 found!");
    }

    // Setup TSL2591
    if (I2CCheckAddress(0x29))
    {
        TSL2591Setup();
        TSL2591GetLux(true, true);
    }
    else
    {
        Serial.println("No TSL2591 found!");
    }

    // Setup SD011 and read values
    ParticlePower(true);
    ParticleSetup();
    ParticleRead(true, true, PM25, PM10);
    ParticlePower(false);

    LoRaWANSetup();
}

void loop()
{
    LoraWANDo();
}