# How to use this repository

1. create secret.h

    #define WIFI_SSID "XXXXXX"

    #define WIFI_PASSWORD "XXXXXX"

2. unzip libraries and move them to global library folder

  tar zxvf lib.tar.gz


# ESP8266 initialization

1. download arduino library(in lib.tar.gz already)

  https://github.com/bportaluri/WiFiEsp

2. refresh ESP8266 firmware

  1. connect UART to computer.

  2. connect 3.3V power to wifi module.

  3. open flashing software in ./ESP8266

  4. choose bin file and set offset as per flash_1.5.png

  5. download the bin files.

3. test AT command

  1. connect UART to computer.

  2. connect 3.3V power to wifi module.

  3. open console debugger in Arduino IDE.

  4. set baudrate 115200 and line ending as: Both NL & CR

  5. type AT or AT+GMR follow by enter, it will print 'OK'.

Then you are ready to go.
