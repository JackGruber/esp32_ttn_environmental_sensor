# ESP32 battery/solar powerd environmental sensor

battery/solar powerd ESP32 to measure temperature, humidity, air pressure and fine dust.

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
* 1x BMP280 sensor
* 1x SDS011 module
* 1x IRL3103PBF
* ...

## Schematic

<img src="img/schematic.png">

### TP4056 modification

The charging LED lights up as soon as the solarpannel supplies some current. However, this will cause the LED to discharge the battery when there is little sunshine.

To prevent this, I have soldered out the charging LED.

<img src="img/TP4056_board_led.jpg">

## Battery levels overview (Measuring and data send interval every 5 minutes)

### No charge current  

<img src="img/power_no_solar.jpg">

`1` = Disconnect solar pannel, `2` = Last data packet sent

### Normal operation

<img src="img/power_operation.jpg">

`1` = First data packet sent after after approx. 6 houre of charge, `2` = Rainy days
