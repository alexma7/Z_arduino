// G my_proj

/*
   TimeRTC.pde
   example code illustrating Time library with Real Time Clock.
   pwm -  5, 6 : 9, 10 : 11  13
   
	Timer0 - используется millis (), micros (), delay () и ШИМ на пинах 5 и 6
	Timer1 - используется для Servo, библиотеки WaveHC и ШИМ на пинах 9 и 10
	Timer2 - используется Tone и ШИМ на пинах 11 и 13

*/
#include <Arduino.h>
#include "NewPing.h"
//#include <Ultrasonic.h>  //Дальнометр SR04

#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

#include "IRremote.h"
#include <DHT.h>
//**************************************************************//
//  Name    : shiftOutCode, Hello World                               
//  Author  : Carlyn Maw,Tom Igoe, David A. Mellis
//  Date    : 25 Oct, 2006   
//  Modified: 23 Mar 2010                                
//  Version : 2.0                                            
//  Notes   : Программа использует один сдвиговый регистр 74HC595
//          : для вывода значений от 0 до 255                           
//****************************************************************
 
//Пин подключен к ST_CP входу 74HC595
int latchPin = 3;
//Пин подключен к SH_CP входу 74HC595
int clockPin = 4;
//Пин подключен к DS входу 74HC595
int dataPin = 2;
 
 
 
void setup() {
  //устанавливаем режим OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}
 
void loop() {


    // устанавливаем синхронизацию "защелки" на LOW
    digitalWrite(latchPin, LOW);
    // передаем последовательно на dataPin
    shiftOut(dataPin, clockPin, MSBFIRST, 255); 
 
    //"защелкиваем" регистр, тем самым устанавливая значения на выходах
    digitalWrite(latchPin, HIGH);
    // пауза перед следующей итерацией
    delay(500);
        digitalWrite(latchPin, LOW);
    // передаем последовательно на dataPin
    shiftOut(dataPin, clockPin, MSBFIRST, 0); 
 
    //"защелкиваем" регистр, тем самым устанавливая значения на выходах
    digitalWrite(latchPin, HIGH);
    // пауза перед следующей итерацией
    delay(500);
  
}
