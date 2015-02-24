# **MQTTDUINO** #

###An MQTT client for Arduinos connected to an esp8266 board


# 1) Connect esp8266 to Arduino
DISCLAIMER! I have NOT much experience in electronics, this is MY connection diagram and it's working well for me, but I don't know if it's 100% correct

![Connection diagram](http://i.imgur.com/7uIHUzb.png)


# 2) Install nodemcu lua firmware on esp8266
I'm using 20150213 version, you can use any version from 20150212, hoping that future releases will not break compatibility with the script
You can find it there: https://github.com/nodemcu/nodemcu-firmware


# 3) Push mqttduino script on esp8266
Copy mqttduino.lua and init.lua on the esp8266
You can do it with LuaLoader: http://benlo.com/esp8266/
Don't forget to reboot the esp8266 after copying files


# 4) The Arduino sketch
The mqttduino.ino is a template for your sketch.
The template is self-explanatory