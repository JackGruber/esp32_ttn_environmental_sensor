#include <Arduino.h>
#include <particle.h>

void setup() {
    Serial.begin(115200);
    Serial.println(F("Starting environment sensor ..."));

    ParticleSetup();
}

void loop() {
    Serial.println("Read");
    ParticleRead(true, true);
    delay(30000);
}