# ESP32 battery/solar powerd environmental sensor

Battery/solar-powered ESP32 TTN (LoraWAN) sensor node for measuring temperature, humidity, air pressure and fine dust.

> ❗❗❗ Project is still under development / documents are not complete ❗❗❗

## Components / BOM

* 1x WROOM32 modul
* 1x TP4056 module
* 1x MCP1700-3302E
* 2x Solar panel 6V 1W (110x60 mm)
* 1x 18650 battery
* 1x 18650 battery holder
* 1x 5V DC-DC boost converter module
* 1x RFM95 module
* 1x BME280 sensor
* 1x SDS011 module
* 1x IRL3103PBF
* Housing IP65 115 x 90 x 55 mm
* Cable Gland PG7
* Standoff/Spacer 20 mm
* ...

## Schematic

<img src="img/schematic.png">

### TP4056 modification

The charging LED lights up as soon as the solarpannel supplies some current. However, this will cause the LED to discharge the battery when there is little sunshine.

To prevent this, I have soldered out the charging LED.

<img src="img/TP4056_board_led.jpg">

## Power consumption

| Mode | Power consumption |
| ---- | ----------------- |
| SDS011 measurement (30 sec) | 175 mA |
| LoRa activity | 7 mA |
| DeepSleep | 52 uA |

### Battery levels overview (Measuring and data send interval every 5 minutes)

#### No charge current  

<img src="img/power_no_solar.jpg">

`1` = Disconnect solar pannel, `2` = Last data packet sent

#### Normal operation

<img src="img/power_operation.jpg">

`1` = First data packet sent after after approx. 6 houre of charge, `2` = Rainy days

## TTN payload decoder

```javascript
function Decoder(bytes, port) {
  var decoded = {};
  
  decoded.vcc = (bytes[0] + 200)/100;

  if(bytes[1] != 255 || bytes[2] != 255)
  {
    decoded.pm25 = ((bytes[1] << (8*1) | bytes[2] << (8*0)) / 10);
  }

  if(bytes[3] != 255 || bytes[4] != 255)
  {
    decoded.pm10 = ((bytes[3] << (8*1) | bytes[4] << (8*0)) / 10);
  }
  
  if(bytes[5] != 255 || bytes[6] != 255)
  {
    decoded.temperature = ((bytes[5]<<24>>16 | bytes[6]) / 10);
  }
  
  if(bytes[7] != 255)
  {
    decoded.humidity = bytes[7];
    decoded.humidity &= ~(1 << 7);
    if(bytes[7] >> 7 == 1) { decoded.humidity +=0.5 }
  }
  
  pressure = (bytes[8] << (8*0) | bytes[9] << (8*1) | bytes[10]  << (8*2)) / 100;
  if(pressure >= 300 && pressure <= 1100) { decoded.pressure = pressure }

  if(bytes[11] != 255) decoded.uvi = bytes[11] / 10;

  return decoded;
}
```
