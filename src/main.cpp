#include <Arduino.h>


#include <AntaresESP32MQTT.h>

#define ACCESSKEY "ecfe802f4d7e8885:01ffe2986b9d9565"
#define WIFISSID "XL-Guest"
#define PASSWORD "Pri[o]rita$"

#define projectName "smartpjuv1"
#define deviceName "xcamp"
float kwh;
float tegangan; 
float daya; 
float arus;
float temp;
float humi;
int modeControl = readMode();

AntaresESP32MQTT antares(ACCESSKEY);
DHT dht(DHTPIN,DHTTYPE);
PZEM004Tv30 pzem(&Serial1,RXD1,TXD1);

RTC_DS3231 rtc;
void setup() {
  Serial.begin(115200);
  // antares.setDebug(true);
  antares.wifiConnection(WIFISSID, PASSWORD);
  antares.setMqttServer();

rtc.begin();
  dht.begin();
rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));  
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
     modeControl = 0;
     writeMode(modeControl);
    if(state == "ON"){
      // modeport1=1;
      // writePort1(modeport1);
      // dimmer.setPower(90);
      digitalWrite(25,HIGH);
      digitalWrite(26,HIGH);
    }else{
      // modeport1 = 0;
      // writePort1(modeport1);
      digitalWrite(25,LOW);
      digitalWrite(26,LOW);
      // dimmer.setPower(0);
    }
  }
}