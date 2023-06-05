#include <Arduino.h>


#include <AntaresESP32MQTT.h>
#include <saveData.h>
#include <parsingWaktu.h>
#define ACCESSKEY "ecfe802f4d7e8885:01ffe2986b9d9565"
#define WIFISSID "XL-Guest"
#define PASSWORD "Pri[o]rita$"
#define outputPin  23
#define zerocross  15
#define projectName "smartpjuv1"
#define deviceName "xcamp"
float kwh;
float tegangan; 
float daya; 
float arus;
float temp;
float humi;
int modeControl = readMode();
// int modeControl = readMode();
int hour;
int minute;
int detik;
String times;
int startHour = readStartHour();
int endHour = readEndHour();
int startMinute = readStartMinute(); 
int endMinute = readEndMinute();
AntaresESP32MQTT antares(ACCESSKEY);
DHT dht(DHTPIN,DHTTYPE);
PZEM004Tv30 pzem(&Serial1,RXD1,TXD1);
bool manual = false;
bool Auto = false;
int modeport1 = readPort1();
int modedimm = readPort2();
dimmerLamp dimmer(outputPin, zerocross); //initialase port for dimmer for ESP8266, ESP32, Arduino due boards

RTC_DS3231 rtc;
void setup() {
  Serial.begin(115200);
  antares.setDebugs(true);
  antares.wifiConnection(WIFISSID, PASSWORD);
  antares.setMqttServer();
  pinMode(25,OUTPUT);
   dimmer.begin(NORMAL_MODE, ON); //dimmer initialisation: name.begin(MODE, STATE) 
  if(modeport1 == 1){
    digitalWrite(25,HIGH);
    dimmer.setPower(90);
  }
  if(modeControl == 2 ){
    dimmer.setPower(40);
    digitalWrite(25,LOW);
  }else if(modeControl == 0){
    dimmer.setPower(90);
  }
  if(modeControl == 1 ){
    Auto = true;
  }else{
    Auto = false;
  }
  rtc.begin();
  dht.begin();
// rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));  
}

void loop() {
  /*
    Check if we're still connected to the MQTT broker/server.
    If disconnected, the device will try to reconnect.
   */
  daya =pzem.power();
  arus = pzem.current();
  tegangan = pzem.voltage(); 
  kwh = pzem.energy(); 
  if(isnan(daya)){
    daya = 0;
  }if(isnan(arus)){
    arus = 0;
  }
  if(isnan(tegangan)){
    tegangan =0;
  }
  if(isnan(kwh)){
    kwh = 0;
  }
  antares.checkMqttConnection();
  // Variable init
 temp = dht.readTemperature();
  humi = dht.readHumidity();
  // Add variable to data storage buffer
  antares.add("temperature", temp);
  antares.add("humidity", humi);
  antares.add("kwh", kwh);
  antares.add("power", daya);
  antares.add("volt", tegangan);
  antares.add("current", arus);
  // Publish and print data
  antares.publish(projectName, deviceName);
  delay(5000);
}
void callback(char topic[], byte payload[], unsigned int length) {
  antares.get(topic, payload, length);
  String mode = antares.getString("mode");
  String state = antares.getString("state");
  if(mode == "MANUAL" || mode =="AUTO" || mode == "DIMMING"){

  Serial.println("Payload: " + antares.getPayload());
  }
  if(mode == "MANUAL"){
  
    if(state == "ON"){
      digitalWrite(25,HIGH);
    }else{
      digitalWrite(25,LOW);
      
    }
  }
}