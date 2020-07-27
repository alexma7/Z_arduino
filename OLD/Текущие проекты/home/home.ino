/* 
   TimeRTC.pde 
   example code illustrating Time library with Real Time Clock. 
  
*/ 
#include <Ultrasonic.h>  //Дальнометр SR04 
#include <avr/pgmspace.h> // ДлЯ Progem запись во флэш 
  
  
//КОНСТАНТЫ 
//Входы 
#define btnSpeac      6 
#define btn      5 
#define micD      7 

//Выходы 
#define lamp      10 
#define micLamp      11 

//Датчик приближения
#define sr04Trig  9 
#define sr04Echo  8 

//74HC595  
#define DS 2 
#define ST_CP 3 
#define SH_CP 4 
const int micA = A0; 
  
int tempLamp = 1;
int inc=0;
int TEMP; 
int sensorValue = 0; 
int val; 
int myInts[10];
byte registers = 0b00000000; 
char ch; 
String str = ""; 
Ultrasonic ultrasonic(sr04Trig, sr04Echo); 
  
  
  
void setup()  { 
      //устанавливаем режим OUTPUT 74HC595 
 /* pinMode(ST_CP, OUTPUT); 
  pinMode(SH_CP, OUTPUT); 
  pinMode(DS, OUTPUT); */
   
   
  pinMode(btnSpeac, INPUT);   //а 5й – входом 
  pinMode(btn, INPUT);   //а 3й – входом 
  pinMode(micD, INPUT);   //а 8й – входом микрофона 
  
  pinMode(lamp, OUTPUT);  //8й вывод будет выходом 
  digitalWrite(lamp, LOW); 
  
  pinMode(micLamp, OUTPUT);  //9й вывод будет выходом 
  digitalWrite(micLamp, LOW); 
   
  //устанавливаем режим OUTPUT 74HC595 
  
/*  pinMode(latchPin, OUTPUT); 
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, OUTPUT); */
  

  
  
  
  
  Serial.begin(115200); 

} 
  
void loop() 
{ 
    //Сдвиговый регистр 
    /*regShift();*/ 
     
     
  /*if (digitalRead(micD) == HIGH) { 
    delay(150); //ожидаем 200милисекунд для повторного хлопка - можно не делать но в моём случае это добавило удобства 
    for (int t = 0; t <= 300; t++) //цикл на проверку второго хлопка по идее на пол секунды но ардуино делает его дольше 
    { 
      delay(1); //для цикла 
       
      if (digitalRead(micD) == HIGH) { //сверяем данные для второго хлопка, если он был то меняем состояние лампы с вкл на выкл и наоборот 
        digitalWrite(micLamp, !digitalRead(micLamp)); //меняем состояние 
        break;//выходим из цикла после второго хлопка 
        delay(200);//для удобства 
      } 
    }*/ 
//Дальнометр 
  /*float dist_cm = ultrasonic.Ranging(1);       // get distance 
  if (dist_cm<10){
	  while(dist_cm<10){
		  dist_cm = ultrasonic.Ranging(1);
		delay(50);  
	  }
	digitalWrite(lamp, !digitalRead(lamp));
	Serial.println(dist_cm); 
	delay(500); 
  }*/
  if ((digitalRead(btn)) and (tempLamp)){
	digitalWrite(lamp, !digitalRead(lamp)); 
	tempLamp=0;
	inc++;
	Serial.println("---------------------"+String(inc)); 
	//delay(100); 
  }
  
  if ((!digitalRead(btn)) and (!tempLamp)){
	Serial.println("0");
	tempLamp=1;  
  }

  
// if (TEMP!=analogRead(micA)){ 
//   TEMP = analogRead(micA); 
//   Serial.println(TEMP); // выводим в порт 
// } 
  
  
  

   

  //********************************************** 
  /*sensorValue = analogRead(analogInPin); 
    Serial.print("Microphone value= "); 
    Serial.println(sensorValue); */ 
  
  //int sensorValue = analogRead(analogInPin) * 5.0 / 1024.0 * 1000.0; // значения в милливольтах 
     
  
  
  
  
  
} 

//Сдвиг через сдвиговый регистр 74HC595 
/*void regShift (){ 
for (int i=0; i<3; i++) { 
    digitalWrite(latchPin, LOW); 
    shiftOut(dataPin, clockPin, LSBFIRST, path[i]); 
    digitalWrite(latchPin, HIGH); 
    delay(1); 
    } 
  for (int i=1; i<2; i++) { 
    digitalWrite(latchPin, LOW); 
    shiftOut(dataPin, clockPin, LSBFIRST, path[2-i]); 
    digitalWrite(latchPin, HIGH); 
    delay(1); 
    } 
} */

//Термистор 
/*double Thermister(int RawADC) { 
    double Temp; 
    Temp = log(((10240000/RawADC) - 10000)); 
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp ); 
    Temp = Temp - 273.15;        // Kelvin to Celcius 
    //Temp = (Temp * 9.0)/ 5.0 + 32.0;        // 1 способ Convert Celcius to Fahrenheit  
    //Temp = (Temp * 1.8) + 32.0;        // 2 способ Convert Celcius to Fahrenheit 
    return Temp; 
} */
//Установить бит в сдвиговый регистр 
void setRegShift(int num) 
{ 
    if (bitRead(registers,num)) 
    { 
        bitWrite(registers, num,0); 
        regShift(registers); 
        Serial.println("/home/hall/ard1relay"+String(num)+"@1%"); 
    } 
    else 
    { 
        bitWrite(registers, num,1); 
        regShift(registers); 
        Serial.println("/home/hall/ard1relay"+String(num)+"@0%"); 
    } 
     
    //Serial.println(analogRead(btns)); 
    delay(400); 
  
} 
//Сдвиг через сдвиговый регистр 74HC595 
void regShift (byte a){ 
    digitalWrite(ST_CP, LOW); 
    shiftOut(DS, SH_CP, MSBFIRST, a); 
    digitalWrite(ST_CP, HIGH); 
    Serial.print("------"); 
    Serial.println(a); 
} 
  
  
 