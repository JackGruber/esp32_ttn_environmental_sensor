#include <Arduino.h>
#include <particle.h>
#include <lorawan.h>
#include <functions.h>
#include <global.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable Brownout Detector
    Serial.begin(115200);
    Serial.println(F("Starting environment sensor ..."));

    PrintResetReason();
    SetupPins();
    // Setup SD011 and read values
    ParticlePower(true);
    ParticleSetup();
    ParticleRead(true, true, PM25, PM10);
    ParticlePower(false);
    
    LoRaWANSetup();
}

void loop() {
    LoraWANDo();
    delay(10);
}