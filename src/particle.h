#pragma once
#ifndef _PARTICLE_H
#define _PARTICLE_H

#include <Arduino.h>

void ParticleSetup(void);
void ParticleRead(bool wakeup, bool sleep, float& pm25, float& pm10);
void ParticleSleep(void);
void ParticleWakeup(bool wait);
void ParticlePower(bool power);

#endif
