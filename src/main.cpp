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

    ReadVBat();

    // Setup BME280 and print values
    BME280Setup();
    BME280PrintValues();

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
    delay(10);
}