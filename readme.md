# Introduction

This is an arduino base sensor with web(port 80) server.

Hardware: Arduino Mega2560, DHT22 sensor, ESP8266 module(wifi) and arduino IO expander card.

It will print some basic info on console(UART0) while initialization, including sensor and wifi info.

Anyone can send a http get request to http://ip/dht22, and arduino will reply below message in json format:

{"dht22":{"status":"succeed","message":{"temperature":"19.20C","humidity":"17.20%"}}}

or below error message:

{"dht22":{"status":"error","message":"Error reading temperature!"}}

**http get request should starts with below string:**

GET /DHT22 HTTP

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
