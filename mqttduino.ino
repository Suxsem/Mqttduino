#define APssid				"HomeWIFI"				//wifi network ssid
#define APpsw				"wifipassword"			//wifi netwrok password
#define MQTTid				"MqttTestClient"		//id of this mqtt client
#define MQTTip				"mqtt.myserver.com"		//ip address or hostname of the mqtt broker
#define MQTTport			1883					//port of the mqtt broker
#define MQTTuser			"client1"				//username of this mqtt client
#define MQTTpsw				"password1"				//password of this mqtt client
#define MQTTalive			120						//keep alive interval (seconds)
#define MQTTretry			10						//time to wait before reconnect if connection drops (seconds)
#define MQTTqos				2						//quality of service for subscriptions and publishes
#define timeout_check		500						//timeout for checking incoming messages (milliseconds)
#define timeout_send		5000					//timeout for subscriptions and publishes (milliseconds)

boolean connected = false;

void onConnected() {	//on connected callback
	mqttSubscribe("hello");	//subscribe to "hello" topic	
}

void onDisconnected() {	//on disconnected callback
}

void onMessage(String topic, String message) {	//new message callback
}

#### DO NOT TOUCH THIS CODE! ####
#define buffer_l 50
char in_buffer[buffer_l + 1];
char cb[1];
boolean success;
void checkComm() {
	if (Serial.find("[(")) {
		Serial.readBytes(cb, 1);
		if (cb[0] == 'r') {
			//ready
			Serial.setTimeout(timeout_check);			
			if (connected) {
				connected = false;
				onDisconnected();
			}			
			Serial.println("connectAP(\"" + String(APssid) + "\", \"" + String(APpsw) + "\")");		
		} else if (cb[0] == 'w') {
			//wifi connected
			Serial.println("mqttInit(\"" + String(MQTTid) + "\", \"" + String(MQTTip) + "\", " + MQTTport + ", \"" + String(MQTTuser)
							+ "\", \"" + String(MQTTpsw) + "\", " + MQTTalive + ", " + MQTTretry + ")");		
		} else if (cb[0] == 'c') {
			//mqtt connected
			connected = true;
			onConnected();
		} else if (cb[0] == 'd') {
			//disconnected
			connected = false;
			onDisconnected();
		} else if (cb[0] == 'm') {
			//new message
			memset(in_buffer, 0, sizeof(in_buffer));
			Serial.readBytesUntil('|', in_buffer, buffer_l);
			String topic = String(in_buffer);
			memset(in_buffer, 0, sizeof(in_buffer));
			Serial.readBytesUntil('|', in_buffer, buffer_l);
			String message = String(in_buffer);
                        while(!success)
                          checkComm();
			onMessage(topic, message);
		} else if (cb[0] == 'p' || cb[0] == 's') {
			success = true;
		}
	}
}
void mqttPublish(String topic, String message, unsigned int retain) {
  	success = false;
        while (!success) {
        	Serial.println("mqttPublish(\"" + topic + "\", \"" + message + "\",  " + MQTTqos + ", " + retain + ")");
        	Serial.setTimeout(timeout_send);
        	checkComm();
        	Serial.setTimeout(timeout_check);
        }
}
void mqttSubscribe(String topic) {
	success = false;
        while(!success) {
	        Serial.println("mqttSubscribe(\"" + String(topic) + "\", " + MQTTqos + ")");
        	Serial.setTimeout(timeout_send);
	        checkComm();
        	Serial.setTimeout(timeout_check);
        }
}
#### END OF UNTOUCHABLE CODE ####

void setup() {
	Serial.begin(9600);	//start serial
  
	while(!connected)	//
		checkComm();	//wait for connection
}

void loop() {
	do					//
		checkComm();	//
	while(!connected);	//check for incoming messages
	
	delay(10000);
	
	mqttPublish("uptime", String(millis()), 0);			//publish new message to "uptime" topic, with no retain
	mqttPublish("sensor", String(analogRead(A0)), 0);	//publish new message to "sensor" topic, with retain
}