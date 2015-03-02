# **MQTTDUINO**

###An MQTT client for Arduinos connected to an esp8266 board
<br />

# **FEATURES**
- Subscribe to topics
- Send messages
- Receive messages
- All qos levels
- Messages retain
- Authentication with username and password
- Last will and testament (in topic [mqtt id]/status)
- Auto reconnect on wifi drop or mqtt broker drop!
- Auto reset esp8266 on lock up!
- VERY easy to use and reliable, low memory footprint
- (TODO) SSL connection (can't do it for now due to memory constraint of nodemcu firmware)

# **USAGE**

### 1) Connect esp8266 to Arduino
DISCLAIMER! I have NOT much experience in electronics, this is MY connection diagram and it's working well for me, but I don't know if it's 100% correct<br />
<br />
![Connection diagram](electrical_connections/connect_to_arduino.png?raw=true)

### 2) Install nodemcu lua firmware on esp8266
I'm using 20150213 version, you can use any version from 20150212, hoping that future releases will not break compatibility with the script<br />
You can find it there: https://github.com/nodemcu/nodemcu-firmware


### 3) Push mqttduino script on esp8266
Copy mqttduino.lua and init.lua on the esp8266<br />
You can do it with LuaLoader: http://benlo.com/esp8266/<br />
Don't forget to reboot the esp8266 after copying files


### 4) The Arduino sketch
The mqttduino.ino is a template for your sketch.<br />
The template is self-explanatory

# WARNING!
- I suggest to increate the Arduino serial buffer size in
```C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\HardwareSerial.h```
adding
```
#define SERIAL_TX_BUFFER_SIZE 100
#define SERIAL_RX_BUFFER_SIZE 100
```
under
```
...
#define SERIAL_TX_BUFFER_SIZE 64
#define SERIAL_RX_BUFFER_SIZE 64
#endif
#endif
```
<br />
- The script CAN'T receive messages or topics that contains the | character because it's used internally as a separator. Please don't use this character in topics or messages.
- When the Arduino is subscribing to a topic or publishing a message, only one message will be queued. (Due to memory constraint)