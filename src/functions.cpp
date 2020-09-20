#include "functions.h"
#include <rom/rtc.h>
#include <io_pins.h>
#include <driver/adc.h>
#include <settings.h>
#include <Wire.h>

void PrintResetReason()
{
  Serial.print("MCUSR: ");
  switch (rtc_get_reset_reason(0))
  {
  case 1:
    Serial.println("POWERON_RESET");
    break; /**<1,  Vbat power on reset*/
  case 3:
    Serial.println("SW_RESET");
    break; /**<3,  Software reset digital core*/
  case 4:
    Serial.println("OWDT_RESET");
    break; /**<4,  Legacy watch dog reset digital core*/
  case 5:
    Serial.println("DEEPSLEEP_RESET");
    break; /**<5,  Deep Sleep reset digital core*/
  case 6:
    Serial.println("SDIO_RESET");
    break; /**<6,  Reset by SLC module, reset digital core*/
  case 7:
    Serial.println("TG0WDT_SYS_RESET");
    break; /**<7,  Timer Group0 Watch dog reset digital core*/
  case 8:
    Serial.println("TG1WDT_SYS_RESET");
    break; /**<8,  Timer Group1 Watch dog reset digital core*/
  case 9:
    Serial.println("RTCWDT_SYS_RESET");
    break; /**<9,  RTC Watch dog Reset digital core*/
  case 10:
    Serial.println("INTRUSION_RESET");
    break; /**<10, Instrusion tested to reset CPU*/
  case 11:
    Serial.println("TGWDT_CPU_RESET");
    break; /**<11, Time Group reset CPU*/
  case 12:
    Serial.println("SW_CPU_RESET");
    break; /**<12, Software reset CPU*/
  case 13:
    Serial.println("RTCWDT_CPU_RESET");
    break; /**<13, RTC Watch dog Reset CPU*/
  case 14:
    Serial.println("EXT_CPU_RESET");
    break; /**<14, for APP CPU, reseted by PRO CPU*/
  case 15:
    Serial.println("RTCWDT_BROWN_OUT_RESET");
    break; /**<15, Reset when the vdd voltage is not stable*/
  case 16:
    Serial.println("RTCWDT_RTC_RESET");
    break; /**<16, RTC Watch dog reset digital core and rtc module*/
  default:
    Serial.println("NO_MEAN");
  }
  Serial.println();
}

void SetupPins()
{
  pinMode(PIN_ENABLE_SD011, OUTPUT);
  pinMode(PIN_VBAT, INPUT);
}

long ReadVBat()
{
  Serial.print("ReadVBat = ");
  long vccraw = 0;
  int read_raw;
  int smaples_ok = 0;
  long vbat = 0;
  long raw = 0;

  for (int smaples = 1; smaples <= 50; smaples++)
  {
    adc2_config_channel_atten(ADC2_CHANNEL_7, ADC_ATTEN_0db);
    esp_err_t r = adc2_get_raw(ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &read_raw);
    if (r == ESP_OK)
    {
      smaples_ok++;
      raw += read_raw;
      delay(10);
    }
    else if (r == ESP_ERR_TIMEOUT)
    {
      Serial.println("ADC2 used by Wi-Fi.");
    }
  }
  if (smaples_ok > 0)
  {
    vccraw = ((1.1 / 4095) * raw * 1000) / smaples_ok;
    vbat = vccraw * ((float)REF_VBAT / (float)REF_VCCRAW_ESP32);
    Serial.print("VCCraw: ");
    Serial.print(vccraw);
    Serial.print(" mV, vBat: ");
    Serial.print(vbat);
    Serial.println(" mV");

    return vbat; // vbat in millivolts
  }
  else
    return 0;
}

void I2CScanner()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning I2C Bus ...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

bool I2CCheckAddress(byte address)
{
  byte error;
  Wire.beginTransmission(address);
  error = Wire.endTransmission();
  if (error == 0)
    return true;
  else
    return false;
}