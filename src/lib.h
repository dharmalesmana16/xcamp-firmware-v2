// #ifndef lib_H
// #define lib_H
#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_TEST_GPS true
#include <Arduino.h>
#include <PZEM004Tv30.h>
#include "RBDdimmer.h"
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <TinyGsmClient.h>  
#include <EEPROM.h>
#include "PubSubClient.h"
#include "ArduinoJson.h"
#define TINY_GSM_DEBUG Serial
#define EEPROMSIZE 200
#define DHTTYPE 22 
#define DHTPIN 4
// #define SENSOR_LDR 27 
#define outputPin  23
#define zerocross  15
String latitude;
String longitude; 
int light; 

HardwareSerial SerialAT(2);  // RX, TX
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm        modem(debugger);
#else
#include <TinyGsmClient.h>
TinyGsm        modem(SerialAT);
#endif

TinyGsmClient modemclient(modem);
PubSubClient  client(modemclient);

class lib {
	public: 
        lib(String accessKey);
//    bool wifiConnection(String SSID, String wifiPassword);
        bool setIndic(bool trueFalse);
        void printDebug(String text);
        // String ipToString(IPAddress ip);
        /* Overloaded functions: Add data to temporary storage */
        void add(String key, int value);
        void add(String key, float value);
        void add(String key, double value);
        void add(String key, String value);
        /* Overloaded functions end */
        void printData();
        void publish(String projectName, String deviceName);
        /* Get subscription callback data*/
        int getInt(String key);
        float getFloat(String key);
        double getDouble(String key);
        String getString(String key);
        /* Get data end */

        String getTopic();
        String getPayload();
        void getGPS();
        void gpsON();
        void getTime();
        void getLightIntesity();
        void setMqttServer();
        void checkMqttConnection();
        void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callbackFunc);
        String get(char* topic, byte* payload, unsigned int length);
        void setSubscriptionTopic();
        bool dhcp;
//		 lib(int wizresetpin);

		void startMQTTConnection();
		void connectAPN();
		void reconnectAPN();
//		void sendMsg(String appName,String deviceName);
//		void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callbackFunc);
		void ethReset();
//		int resetwizpin;
		bool rdy;
	// 	String _access;
		// String access;
	private:
        const char* _mqttServer = "mqtt.antares.id";
	    const int _mqttPort = 1883;
	    const int _secureMqttPort = 8883;
	    bool _debug = true;
        const char* _apn = "M2MAUTOTRONIC";
        const char* _gprsUser = "";
        const char* _gprsPass = "";
//	    char* _wifiSSID;
//	    char* _wifiPass;
	    String _accessKey;
	    String _jsonDataString = "{}";
	    String _jsonSubDataString;
	    String _subscriptionTopic;
	    String _receivedTopic;
};
// #include "lib.h"
// #include "config.h"
// #include <Ethernet.h>
//lib::lib(int wizresetpin) {
//    resetwizpin = wizresetpin;
//}
bool rdy = false;
lib::lib(String accessKey) {
    _accessKey = accessKey;
}
void lib::setSubscriptionTopic() {
    _subscriptionTopic = "/oneM2M/req/" + _accessKey + "/antares-cse/json";
}
int lib::getInt(String key) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonSubDataString);
    return object[key];
}

float lib::getFloat(String key) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonSubDataString);
    return object[key];
}

double lib::getDouble(String key) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonSubDataString);
    return object[key];
}


String lib::getString(String key) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonSubDataString);
    return object[key];
}
void lib::setCallback(std::function<void(char*, uint8_t*, unsigned int)> callbackFunc) {
    client.setCallback(callbackFunc);
}
void lib::publish(String projectName, String deviceName) {
    String topic = "/oneM2M/req/" + _accessKey + "/antares-cse/json";
    String finalData;

    _jsonDataString.replace("\"", "\\\"");


    finalData += "{";
    finalData += "\"m2m:rqp\": {";
    finalData += "\"fr\": \"" + _accessKey +"\",";
    finalData += "\"to\": \"/antares-cse/antares-id/" + projectName + "/" + deviceName + "\",";
    finalData += "\"op\": 1,";
    finalData += "\"rqi\": 123456,";
    finalData += "\"pc\": {";
    finalData += "\"m2m:cin\": {";
    finalData += "\"cnf\": \"message\",";
    finalData += "\"con\": \""+ _jsonDataString + "\"";
    finalData += "}";
    finalData += "},";
    finalData += "\"ty\": 4";
    finalData += "}";
    finalData += "}";

    // DynamicJsonBuffer jsonBuffer;
    // JsonObject& object = jsonBuffer.parseObject(finalData);
    // object.prettyPrintTo(Serial);

    char finalDataChar[finalData.length() + 1];
    char topicChar[topic.length() + 1];

    finalData.toCharArray(finalDataChar, finalData.length() + 1);
    topic.toCharArray(topicChar, topic.length() + 1);

    _jsonDataString = "{}";

    client.publish(topicChar, finalDataChar);
}

void lib::checkMqttConnection() {
    _subscriptionTopic = "/oneM2M/resp/antares-cse/" + _accessKey + "/json";
    if (!modem.isNetworkConnected()){
        connectAPN();
    }
    if(!client.connected()) {
        while(!client.connected()) {
            printDebug("[ANTARES] Attempting MQTT connection...\n");

            String clientId = _accessKey;

            char clientIdChar[clientId.length() + 1];
            clientId.toCharArray(clientIdChar, clientId.length() + 1);

            if(client.connect(clientIdChar)) {
                printDebug("[ANTARES] Connected! Client ID:");
                printDebug(clientIdChar);
                printDebug("\n");
                char subscriptionTopicChar[_subscriptionTopic.length() + 1];
                _subscriptionTopic.toCharArray(subscriptionTopicChar, _subscriptionTopic.length() + 1);

                Serial.println();
                Serial.print("[ANTARES] Subscribe Topic: ");
                Serial.println(subscriptionTopicChar);

                client.publish(subscriptionTopicChar, "connect!");
                client.subscribe(subscriptionTopicChar);
            }
            else {
                printDebug("[ANTARES] Failed, rc=" + String(client.state()) + ", Will try again in 5 secs.\n");
                delay(5000);
            }
        }
    }
    client.loop();
}

void lib::printDebug(String text) {

        Serial.print(text);
    
}
void lib::setMqttServer() {
        printDebug("[ANTARES] Setting MQTT server \"" + String(_mqttServer) + "\" on port " + String(_mqttPort) + "\n");
        client.setServer(_mqttServer, _mqttPort);
}
void lib::add(String key, int value) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonDataString);
    object[key] = value;
    String newInsert;
    object.printTo(newInsert);
    _jsonDataString = newInsert;
}

void lib::add(String key, float value) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonDataString);
    object[key] = value;
    String newInsert;
    object.printTo(newInsert);
    _jsonDataString = newInsert;
}

void lib::add(String key, double value) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonDataString);
    object[key] = value;
    String newInsert;
    object.printTo(newInsert);
    _jsonDataString = newInsert;
}

void lib::add(String key, String value) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(_jsonDataString);
    object[key] = value;
    String newInsert;
    object.printTo(newInsert);
    _jsonDataString = newInsert;
}

void lib::printData() {
    printDebug("[ANTARES] " + _jsonDataString + "\n");
}
void lib::startMQTTConnection(){
      client.setServer(_mqttServer, _mqttPort);
    //   client.setCallback(); 
}
void lib::gpsON(){
 modem.enableGPS();
  delay(30000L);
}
void lib::connectAPN(){
String modemInfo = modem.getModemInfo();
   Serial.print("Modem: ");
   Serial.println(modemInfo);
   Serial.print("Waiting for network...");
   if (!modem.waitForNetwork(240000L)) {
      Serial.println(" fail");
      delay(10000);
      return;
   }
   delay(5000);
         Serial.println(" OK");

   if (modem.isNetworkConnected()) {
    Serial.println("Network connected");
  }

  Serial.print(F("Connecting to "));
  Serial.print(_apn);
    Serial.print(F("Connecting to "));
  Serial.print(_apn);
  if (!modem.gprsConnect(_apn, _gprsUser, _gprsPass)) {
    Serial.println("Fail to connect");
    delay(10000);
    return;
  }

  Serial.println(" success");

  if (modem.isGprsConnected()) { 
    Serial.println("GPRS connected"); 
    }

}
void lib::reconnectAPN(){
	
   if (!modem.isNetworkConnected()){
      connectAPN();
   }
}
String lib::get(char* topic, byte* payload, unsigned int length) {
    _receivedTopic = String(topic);

    String payloadString;
    for(int i = 0; i < length; i++) {
        payloadString += char(payload[i]);
    }

    DynamicJsonBuffer jsonBuffer;
    JsonObject& object = jsonBuffer.parseObject(payloadString);
    String parsedString = object["m2m:rsp"]["pc"]["m2m:cin"]["con"];
    _jsonSubDataString = parsedString;

    return _jsonSubDataString;
}

String lib::getTopic() {
    return _receivedTopic;
}

String lib::getPayload() {
    return _jsonSubDataString;
}

// void lib::getGPS(){

// }
// void lib::getLightIntesity(){
//     light = analogRead(SENSOR_LDR);
//     // light = random(23,40);
// }

