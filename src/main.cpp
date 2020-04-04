#include <Arduino.h>
#include <particle.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable Brownout Detector
    Serial.begin(115200);
    Serial.println(F("Starting environment sensor ..."));

    ParticleSetup();
}

void loop() {
    Serial.println("Read");
    ParticleRead(true, true);
    delay(30000);
}