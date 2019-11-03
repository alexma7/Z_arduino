#include <PCF8574.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);  

//КОНСТАНТЫ
  //PCF8574
    #define PCFBtn  4
  //Конец.PCF8574
  
  //74HC595 
    /*#define DS  2
    #define ST_CP  3
    #define SH_CP  4*/
    int DS = 2;
  int ST_CP = 3;
  int SH_CP = 4;
  //Конец.74HC595

//Переменные
  byte registers = 0b00000000;

PCF8574 expander;

void setup() {
    Serial.begin(9600);
    Serial.println("Start");
//PCF8574
  expander.begin(0x20);
  expander.pinMode(PCFBtn, INPUT);
  expander.pinMode(1, INPUT);
  //expander.pullDown(PCFBtn);
//Конец.PCF8574

//LCD
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.print("PUSH?");
//Конец LCD

//устанавливаем режим OUTPUT 74HC595
  pinMode(ST_CP, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(DS, OUTPUT);
//Конец. 74HC595
  
}

void loop() {
  // Код для PCF8574
  byte value = expander.digitalRead(4);
  byte value1 = expander.digitalRead(1);
  byte value2 = expander.digitalRead(2);
  //Serial.println(value, DEC);
  //delay(100);
  // Конец кода для PCF8574

  //LCD
  lcd.setCursor(0, 1);
  if (value==1)
  {
  lcd.print("YES");
  }
  else
  {
  lcd.print("NO ");
  Serial.println("4-0");
  }
  
  if (value1==1)
  {
  lcd.clear();
  Serial.println("1-1");
  }
  
   if (value2==1)
  {
  lcd.print("YES");
  Serial.println("2-1");
  }
  //Конец LCD

  //74HC595 
  setRegShift(1, value2);
  //Конец. 74HC595 


  
delay(100);
}




//------БИБЛИОТЕКИ--------------БИБЛИОТЕКИ------------БИБЛИОТЕКИ-------------БИБЛИОТЕКИ---------------БИБЛИОТЕКИ-------------

//Установить бит в сдвиговый регистр
void setRegShift(int num, int val)
{
  bitWrite(registers, num,val);
  //regShift(registers);
  digitalWrite(ST_CP, LOW);
  shiftOut(DS, SH_CP, MSBFIRST, registers);
  digitalWrite(ST_CP, HIGH);
  }
//Сдвиг через сдвиговый регистр 74HC595
/*void regShift (byte a){
  digitalWrite(ST_CP, LOW);
    shiftOut(DS, SH_CP, MSBFIRST, a);
    digitalWrite(ST_CP, HIGH);
  //Serial.print("------");
  //Serial.println(a);
}*/
