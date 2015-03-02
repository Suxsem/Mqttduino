#define APssid              "HomeWIFI"              //wifi network ssid
#define APpsw               "wifipassword"          //wifi netwrok password
#define MQTTid              "MqttTestClient"        //id of this mqtt client
#define MQTTip              "mqtt.myserver.com"     //ip address or hostname of the mqtt broker
#define MQTTport            1883                    //port of the mqtt broker
#define MQTTuser            "client1"               //username of this mqtt client
#define MQTTpsw             "password1"             //password of this mqtt client
#define MQTTalive           120                     //mqtt keep alive interval (seconds)
#define MQTTretry           10                      //time to wait before reconnect if connection drops (seconds)
#define MQTTqos             2                       //quality of service for subscriptions and publishes
#define esp8266reset        A5                      //arduino pin connected to esp8266 reset pin (analog pin suggested due to higher impedance)
#define esp8266alive        40                      //esp8266 keep alive interval (reset board if fail) (seconds)
boolean connected = false;

void onConnected() {                                //on connected callback
    mqttSubscribe("hello");                         //subscribe to "hello" topic
}

void onDisconnected() {                             //on disconnected callback
}

void onMessage(String topic, String message) {      //new message callback
}

//  ####    DO NOT TOUCH THIS CODE!    ####
#define buffer_l 50
#define replyTimeout 5000
char in_buffer[buffer_l + 1];
char cb[1];
boolean success;
boolean messageQueued = false;
unsigned long lastAliveCheck = 0;
void checkComm() {
    if (millis() - lastAliveCheck > esp8266alive * 2000UL || lastAliveCheck == 0) {
        pinMode(esp8266reset, OUTPUT);
        delay(50);
        pinMode(esp8266reset, INPUT);
        lastAliveCheck = millis();
        connected = false;        
    }
    if (Serial.find("[(")) {
        Serial.readBytes(cb, 1);
        if (cb[0] == 'r') {
            //ready
            if (connected) {
                connected = false;
                onDisconnected();
            }
            lastAliveCheck = millis();            
            Serial.println("startAlive(" + String(esp8266alive) + ")");
            Serial.flush();
            Serial.println("connectAP(\"" + String(APssid) + "\", \"" + String(APpsw) + "\")");
            Serial.flush();
        } else if (cb[0] == 'a') {
            lastAliveCheck = millis();
            checkComm();
        } else if (cb[0] == 'w') {
            //wifi connected
            Serial.println("mqttInit(\"" + String(MQTTid) + "\", \"" + String(MQTTip) + "\", " + MQTTport + ", \"" + String(MQTTuser)
                            + "\", \"" + String(MQTTpsw) + "\", " + MQTTalive + ", " + MQTTretry + ")");
            Serial.flush();
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
            if (messageQueued)
                return;
            if (!success)
                messageQueued = true;
            memset(in_buffer, 0, sizeof(in_buffer));
            Serial.readBytesUntil('|', in_buffer, buffer_l);
            String topic = String(in_buffer);
            memset(in_buffer, 0, sizeof(in_buffer));
            Serial.readBytesUntil('|', in_buffer, buffer_l);
            String message = String(in_buffer);
            waitForSuccess();
            onMessage(topic, message);
            messageQueued = false;
        } else if (cb[0] == 'p' || cb[0] == 's') {
            success = true;
        }
    }
}
void waitForSuccess() {
    unsigned long started = millis();
    while (!success) {
        if (!connected || millis() - started > replyTimeout) {
            success = true;
            break;
        }
        checkComm();
    }
}
void mqttPublish(String topic, String message, byte retain) {
    if (!connected)
        return;
    success = false;
    Serial.println("mqttPublish(\"" + topic + "\", \"" + message + "\",  " + MQTTqos + ", " + retain + ")");                
    Serial.flush();
    waitForSuccess();
}
void mqttSubscribe(String topic) {
    if (!connected)
        return;
    success = false;
    Serial.println("mqttSubscribe(\"" + String(topic) + "\", " + MQTTqos + ")");
    Serial.flush();
    waitForSuccess();
}
//  ####    END OF UNTOUCHABLE CODE    ####

void setup() {
    Serial.begin(9600);                                 //
    Serial.setTimeout(500)                              //start serial

    while(!connected)                                   //
        checkComm();                                    //wait for first connection
}

void loop() {
    do                                                  //
        checkComm();                                    //
    while(!connected);                                  //check for incoming messages
    
    delay(10000);
    
    mqttPublish("uptime", String(millis()), 0);         //publish new message to "uptime" topic, with no retain
    mqttPublish("sensor", String(analogRead(A0)), 1);   //publish new message to "sensor" topic, with retain
}