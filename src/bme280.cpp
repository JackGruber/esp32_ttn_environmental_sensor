#include "bme280.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <global.h>

void BME280Setup(void)
{
    if (!BME280.begin(0x76, &Wire))
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1)
            ;
    }

    BME280.setSampling(Adafruit_BME280::MODE_FORCED,
                       Adafruit_BME280::SAMPLING_X1, // temperature
                       Adafruit_BME280::SAMPLING_X1, // pressure
                       Adafruit_BME280::SAMPLING_X1, // humidity
                       Adafruit_BME280::FILTER_OFF);
}

void BME280PrintValues(void)
{
    Serial.print("Temperature = ");
    Serial.print(BME280.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(BME280.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(BME280.readHumidity());
    Serial.println(" %");

    Serial.println();
}

float BME280ReadTemperature(void)
{
    float val = BME280.readTemperature();
    Serial.print("Temperature = ");
    Serial.print(val);
    Serial.println(" °C");
    return val;
}

float BME280ReadHumidity(void)
{
    float val = BME280.readHumidity();
    Serial.print("Humidity = ");
    Serial.print(val);
    Serial.println(" %");
    return val;
}

float BME280ReadPressure(void)
{
    float val = BME280.readPressure() / 100.0F;
    Serial.print("Pressure = ");
    Serial.print(val);
    Serial.println(" Pa");
    return val;
}