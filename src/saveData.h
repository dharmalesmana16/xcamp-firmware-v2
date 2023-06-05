#include <Arduino.h>
#include <EEPROM.h>
int addr_id = 6;
//==========WRITE==============
void writeStartHour(int &value){
    EEPROM.write(0,value);
    EEPROM.commit();
}
void writeStartMinute(int &value){
    EEPROM.write(1,value);
    EEPROM.commit();
}
void writeEndHour(int &value){
    EEPROM.write(2,value);
    EEPROM.commit();
}
void writeEndMinute(int &value){
    EEPROM.write(3,value);
    EEPROM.commit();
}
void writeMode(int &value){
    EEPROM.write(4,value);
    EEPROM.commit();
}
void writePort1(int &value){
    EEPROM.write(5,value);
    EEPROM.commit();
}
void writePort2(int &value){
    EEPROM.write(6,value);
    EEPROM.commit();
}
void writePort3(int &value){
    EEPROM.write(7,value);
    EEPROM.commit();
}
//=============================

//=========READING=============


int readStartHour(){
    int StartH = EEPROM.read(0);
    return StartH;
}
int readStartMinute(){
    int StartM = EEPROM.read(1);
    return StartM;
}
int readEndHour(){
    int EndH = EEPROM.read(2);
    return EndH;
}
int readEndMinute(){
    int EndM = EEPROM.read(3);
    return EndM;
}
int readMode(){
    int Mode = EEPROM.read(4);
    return Mode;
}
int readPort1(){
    int Mode = EEPROM.read(5);
    return Mode;
}
int readPort2(){
    int Mode = EEPROM.read(6);
    return Mode;
}
int readPort3(){
    int Mode = EEPROM.read(7);
    return Mode;
}
//============================

//=========StringWrite=============
void stringWrite(String &str){
    EEPROM.writeString(addr_id, str);
}
//=================================

//=========StringRead==============
String stringRead(){
    String str = EEPROM.readString(addr_id);
    return str;
}
//=================================