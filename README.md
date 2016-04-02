# WIO

Secluded.io weather station that pushes to http://openweathermap.org/
© 2016 Secluded.io LLC
Created by Skip Christian and Steve Dalton. http://secluded.io

## Building the code

- Get latest Arduino IDE (I am using 1.6.8)
- Install board for ESP8266 using these instructions: https://github.com/esp8266/Arduino#installing-with-boards-manager
- cd to ~/Arduino directory
- clone this repo using "git clone git@github.com:secluded/WIO.git"
- In Arduino library manager look for Adafruit BMP085 libary and the Adafruit DHT library. We are just using regular libraries for now (not unified)
- Open WIO.ino in Arduino IDE and compile

## Customising for your station

- Add your Wifi SSID/Password
- Add you user/password for openweather API using base64 instructions in code
- Add station ID and your GPS coordinates.
- Turn it all on and wait a few minutes and you should see updates for your station on openweathermap

