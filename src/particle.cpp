#include "particle.h"
#include "SdsDustSensor.h"
#include <io_pins.h>

SdsDustSensor sds(PIN_SDS011_RX, PIN_SDS011_TX);

void ParticleSetup(void)
{
  delay(500);

  sds.begin();
  sds.setQueryReportingMode(); // ensures sensor is in 'query' reporting mode  
  
  Serial.println(sds.queryFirmwareVersion().toString());
  Serial.println(sds.queryReportingMode().toString()); 
  Serial.println(sds.queryWorkingState().toString()); 
  Serial.println(sds.queryWorkingPeriod().toString()); 
}

void ParticleRead(bool wakeup = true,bool sleep = false)
{
  // Wakeup and wait 30 sec
  if(wakeup == true)
  {
     ParticleWakeup(true);
  }

  PmResult pm = sds.queryPm();
  if (pm.isOk()) 
  {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25);
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10);
  } 
  else 
  {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }

  if(sleep == true)
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
  
  if(wait == true)
  {
    delay(30000);
  }
}

void ParticlePower(bool power)
{
    digitalWrite(PIN_ENABLE_SD011, power);
}