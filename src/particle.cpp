#include "particle.h"
#include "SdsDustSensor.h"
#include <settings.h>
#include <io_pins.h>
#include <power.h>

SdsDustSensor sds(SDS011_SERIAL);

void ParticleSetup(void)
{
  delay(600);

  sds.begin();
  sds.setQueryReportingMode(); // ensures sensor is in 'query' reporting mode

  Serial.println(sds.queryFirmwareVersion().toString());
  Serial.println(sds.queryReportingMode().toString());
  Serial.println(sds.queryWorkingState().toString());
  Serial.println(sds.queryWorkingPeriod().toString());
}

void ParticleRead(bool wakeup, bool sleep, float &pm25, float &pm10)
{
  // Wakeup and wait
  if (wakeup == true)
  {
    ParticleWakeup(true);
  }

  PmResult pm = sds.queryPm();
  if (pm.isOk())
  {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25);
    pm25 = pm.pm25;
    Serial.print(", PM10 = ");
    pm10 = pm.pm10;
    Serial.println(pm.pm10);
  }
  else
  {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }

  if (sleep == true)
  {
    ParticleSleep();
  }
}

void ParticleSleep(void)
{
  WorkingStateResult state = sds.sleep();
  if (state.isWorking())
  {
    Serial.println("Problem with sleeping the sensor.");
  }
  else
  {
    Serial.println("Sensor is sleeping");
  }
}

void ParticleWakeup(bool wait = true)
{
  sds.wakeup();

  if (wait == true)
  {
    PowerLightSleepTimer(30);
  }
}

void ParticlePower(bool power)
{
  digitalWrite(PIN_ENABLE_SD011, power);
  if (power == true)
  {
    delay(600);
  }
}