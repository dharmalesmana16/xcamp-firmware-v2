#include <Arduino.h>
#include "lib.h"
#include "parsingWaktu.h"
#include "saveData.h"
#define RXD1 32  
#define TXD1 33 
String ACCESSKEY = "ecfe802f4d7e8885:01ffe2986b9d9565";
const String  projectName = "smartpjuv1";
const String deviceName = "xcamp";
int hour;
int minute;
int detik;
// String times;
int startHour = readStartHour();
int endHour = readEndHour();
int startMinute = readStartMinute(); 
int endMinute = readEndMinute();
RTC_DS3231 rtc;

float kwh;
float tegangan; 
float daya; 
float arus;
float temp;
float humi;
String waktuMulai;
String waktuAkhir;
bool manual = false;
bool Auto = false;
char msg[300];
// int modeControl = readMode();
DHT dht(DHTPIN,DHTTYPE);
lib mylib(ACCESSKEY);
PZEM004Tv30 pzem(Serial1,RXD1,TXD1);

// dimmerLamp dimmer(outputPin, zerocross); //initialase port for dimmer for ESP8266, ESP32, Arduino due boards

int port1 = 25;
int port2 = 26;
// int port3 = 27;
// int modeport1 = readPort1();
// int modedimm = readPort2();
void getPZEM();
void temphumi();
void callback(char topic[], byte payload[], unsigned int length);
void do_actions(const char* message);
void setup() {
  Serial.begin(115200);
  // Serial1.begin(115200);
  SerialAT.begin(115200);
  pinMode(25,OUTPUT);
  // pinMode(26,OUTPUT);
  // mylib.gpsON();
  // dimmer.begin(NORMAL_MODE, ON); //dimmer initialisation: name.begin(MODE, STATE) 
  // if(modeport1 == 1){
  //   digitalWrite(port1,HIGH);
  //   dimmer.setPower(90);
  // }
  // if(modeControl == 2 ){
  //   dimmer.setPower(40);
  //   digitalWrite(port1,LOW);
  // }else if(modeControl == 0){
  //   dimmer.setPower(90);
  // }
  // if(modeControl == 1 ){
  //   Auto = true;
  // }else{
  //   Auto = false;
  // }
  
  mylib.connectAPN();
  mylib.startMQTTConnection();
  mylib.setCallback(callback);
  rtc.begin();

  dht.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  }

void loop() {
  DateTime now = rtc.now();
  hour = now.hour();
  minute = now.minute();
  detik = now.second();
  Serial.println(hour);
  Serial.println(minute);
//   int startHour = readStartHour();
// int endHour = readEndHour();
// int startMinute = readStartMinute(); 
// int endMinute = readEndMinute();
  // Serial.println(startHour);
  // Serial.println(endHour);
  // Serial.println(startMinute);
  // Serial.println(endMinute);
  // modeControl= readMode();
  // Serial.println(modeControl);
//   if(modeControl == 1 ){
//     Auto = true;
//   }else{
//     Auto = false;
//   }
 if(Auto){
   if(startHour>endHour){
      Serial.println("Next Day");
      if(hour>=startHour){
        if(minute>=startMinute){
          digitalWrite(25,LOW);
          // dimmer.setPower(90);
          Serial.println("LAMP MODE ON");
        }
      }else if(hour-12>=endHour){
        if(minute>=endMinute){
          digitalWrite(25,HIGH);
          Serial.println("LAMP MODE OFF");
        }
      }else{
                  digitalWrite(25,HIGH);

          Serial.println("LAMP MODE OFF");
      }  
    }
    //ketika jam mulai sama dengan jam akhir maka logikanya bermain di menit
    else if (startHour==endHour){
      Serial.println("1JAM");
      if(hour==startHour){
        if (minute>=startMinute && minute<endMinute){
          // digitalWrite(modeport1,HIGH);
          // digitalWrite(modeport2,HIGH);
          digitalWrite(25,LOW);
          Serial.println("LAMP PORT 1 , PORT 2 ,PORT 3: MODE ON");
        }
        else if (minute>=endMinute){
          // digitalWrite(modeport1,LOW);
          // digitalWrite(modeport2,LOW);
                  digitalWrite(25,HIGH);
          Serial.println("LAMP MODE OFF");
        }
      }else{
          // digitalWrite(modeport1,LOW);
          // digitalWrite(modeport2,LOW);
                  digitalWrite(25,HIGH);
          Serial.println("LAMP MODE OFF");
      }
    }
    //Ketika jam mulai kurang dari jam akhir maka dikatakan hari masih hari yang sama 
    else if(startHour<endHour){
      Serial.println("SameDay");
      if(hour>=startHour && hour<=endHour){
        if(minute>=startMinute){ // mau di improve dengan startMinute >= minute
          // digitalWrite(modeport1,HIGH);
          // digitalWrite(modeport2,HIGH);
                  digitalWrite(25,LOW);
          Serial.println("Lamp: ON");
        }
      }else if (hour>=endHour){
        if(minute>endMinute){
          // digitalWrite(modeport1,LOW);
          // digitalWrite(modeport2,LOW);
                  digitalWrite(25,HIGH);
          Serial.println("Lamp: OFF");
        }
      }else{
          // digitalWrite(modeport1,LOW);
          // digitalWrite(modeport2,LOW);
                  digitalWrite(25,HIGH);
          Serial.println("Lamp: OFF");
      }
    }
    }
    // if(modeport2 == 1){
    //   digitalWrite(port2,HIGH);
    // }else{
    //   digitalWrite(port2,LOW);
    // }
    
  // }
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
   mylib.checkMqttConnection();

//        float lat,lon;
//  if (modem.getGPS(&lat, &lon)) {
//       Serial.printf("lat:%f lon:%f\n", lat, lon);
//       lat = lat;
//       lon = lon;
//     } else {
//       lat = 0;
//       lon = 0;
//       Serial.println("Tidak Dapat GPS ");

//     }
//   mylib.getLightIntesity();

  temp = dht.readTemperature();
  humi = dht.readHumidity();
  mylib.add("temperature", temp);
  mylib.add("humidity", humi);
  mylib.add("kwh", kwh);
  mylib.add("power", daya);
  mylib.add("volt", tegangan);
  mylib.add("current", arus);
  // mylib.add("light", light);
  // mylib.add("lat", lat);
  // mylib.add("long", lon);
  mylib.add("status", "monitoring");
  mylib.add("id_device", "1");
  mylib.publish(projectName, deviceName);
  delay(2000);
  }

void callback(char topic[], byte payload[], unsigned int length) {
  mylib.get(topic, payload, length);
  String mode = mylib.getString("mode");
  String state = mylib.getString("state");
  if(mode == "MANUAL" || mode =="AUTO" || mode == "DIMMING"){
  Serial.println("Payload: " + mylib.getPayload());
  }
  if(mode == "MANUAL"){
    //  modeControl = 0;
    //  writeMode(modeControl);
      Auto = false;

    if(state == "OFF"){
      digitalWrite(25,HIGH);
    }else if(state=="ON"){
      digitalWrite(25,LOW);
      
    }else{
      digitalWrite(25,LOW);
    }
  }
  else if(mode== "AUTO"){
  String waktuMulai = mylib.getString("started_at");
    String waktuAkhir = mylib.getString("ended_at");
  //   Serial.println(waktuMulai);
  //   Serial.println(waktuAkhir);
    int startHour = hourGet(waktuMulai);
    writeStartHour(startHour);
    int startMinute = minuteGet(waktuMulai);
    writeStartMinute(startMinute);
    int endHour = hourGet(waktuAkhir);
    writeEndHour(endHour);
    int endMinute = minuteGet(waktuAkhir);
    writeEndMinute(endMinute); 
  Auto = true;
  //   modeControl=1;
  //   writeMode(modeControl);
  }
  // String temps = mylib.getString("temp");
  // String state = mylib.getString("state");
  // String port = mylib.getString("port");
  // Serial.println(temps);
  // Serial.println(mode);
  // if(mode == "MANUAL"){
  //   Auto = false;
  //   modeControl = 0;
  //   writeMode(modeControl);
  
  //     if(state == "ON"){
  //     modeport1 = 1;
  //     Auto = false;
  //     int modeControl = 0;
  //     writePort1(modeport1);
  //     digitalWrite(port1,HIGH); 
  //     dimmer.setPower(90);
  //     }else{
  //       modeport1 = 0;
  //       writePort1(modeport1);
  //       digitalWrite(port1,LOW);
  //       dimmer.setPower(0); 
  //     }
  // }
  // else{
  
  // }

}