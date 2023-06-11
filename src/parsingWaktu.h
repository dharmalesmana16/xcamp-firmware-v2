#include <Arduino.h>

char c[10];

int hourGet(String str){
    int len = str.length();
    str.toCharArray(c,len);
    int jam_msg = (((int)c[0]-48)*10)+((int)c[1]-48);
    return jam_msg;
}

int minuteGet(String str){
    int len = str.length();
    str.toCharArray(c,len);
    int menit_msg = (((int)c[3]-48)*10)+((int)c[4]-48);
    return menit_msg;
}   