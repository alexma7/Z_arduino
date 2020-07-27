/*
   TimeRTC.pde
   example code illustrating Time library with Real Time Clock.

*/
#include <Ultrasonic.h>  //Дальнометр SR04
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <avr/pgmspace.h> // ДлЯ Progem запись во флэш


//КОНСТАНТЫ
//Входы
#define btn2      2
#define btn      3
#define micD      7
#define sr04Trig  8
#define sr04Echo  7
//Выходы
//#define speacRelay      8
//#define micLamp      9
const int micA = A0;

int TEMP;
int sensorValue = 0;
int val;
int myInts[10];
char ch;
String str = "";
 Ultrasonic ultrasonic(sr04Trig, sr04Echo);
 
 //74HC595 
 /*int latchPin = 5;
//Пин подключен к SH_CP входу 74HC595
int clockPin = 6;
//Пин подключен к DS входу 74HC595
int dataPin = 4;*/
byte path[3]= {
  B01111111,
  B10111111,
  B11011111
  };
  
  //Rgb светодиод
/*const int RedPin = 1;
const int GreenPin = 2;
const int BluePin = 3;*/
int redPin = 9;
int greenPin = 10;
int bluePin = 11;

String redNum;
String greenNum;
String blueNum;
 byte color[][3]  = {
  {119, 221, 231},
  {227, 38, 54},
  {153, 102, 204},
  {106, 90, 205},
  {0, 49, 83},
  {119, 221, 231},
  {176, 63, 53},
  {175, 238, 238},
  {152, 251, 152},
  {255, 220, 51},
  {42, 141, 156},
  {0, 155, 118},
  {144, 0, 32},
  {144, 0, 32},
  {144, 0, 32},
  {144, 0, 32},
  {144, 0, 32},
  {144, 0, 32},
  {144, 0, 32},
  {144, 0, 32},
  {248, 23, 62}
  };
  int collorRow = sizeof(color) /3; //Кол-во строк массива, 3 столбца 



void setup()  {
  pinMode(btn2, INPUT);   //а 2й – входом
  pinMode(btn, INPUT);   //а 3й – входом
  pinMode(micD, INPUT);   //а 8й – входом микрофона

  //pinMode(speacRelay, OUTPUT);  //8й вывод будет выходом
  //digitalWrite(speacRelay, LOW);

  //pinMode(micLamp, OUTPUT);  //9й вывод будет выходом
  //digitalWrite(micLamp, LOW);
  
  //устанавливаем режим OUTPUT 74HC595
  /*
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  */
  //RGB
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);


 

  Serial.begin(115200);
  while (!Serial) ; // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
}

void loop()
{
  //Сдвиговый регистр
  /*regShift();*/
  
  //RGB светодиод
    if (digitalRead(btn) == LOW) {
		for (long  t = 0; t < collorRow; t++) //цикл на перебор строк массива
		{
		  setColor(t);
		  for (int i = 0; i < 65000; i++)
		  {
			delay(4);
			if (digitalRead(btn) == HIGH) //Если нажата кнопка, то режим рандома
				break;
			  //Дальнометр
			if (ultrasonic.Ranging(1) <25 /*|| digitalRead(micD) == HIGH*/)
			{
				delay(2000);
				break;
			}
			//Зациклить на цвете, если нажата кнопка 2
			if (digitalRead(btn2) == HIGH)
				for(;;)
				{
					delay(50);
					if (digitalRead(btn2) == LOW)
						break;
				}
			//ОТЛАДКА Дальнометр в сериал
			//float dist_cm = ultrasonic.Ranging(1);       // get distance
			//Serial.println(dist_cm);
		  }
		}
    }
    else
    {
      setColor(random(0, collorRow-1 )); //Устанавливаем случайный цвет
      for (int i = 0; i < 8000; i++)
      {
		delay(30);
		if (digitalRead(btn) == LOW)
		break;
		//Дальнометр
		if (ultrasonic.Ranging(1) <25 /*|| digitalRead(micD) == HIGH*/)
		{
			delay(2000);
			break;
		}
		//Зациклить на цвете, если нажата кнопка 2
			if (digitalRead(btn2) == HIGH)
				for(;;)
				{
					delay(50);
					if (digitalRead(btn2) == LOW)
						break;
				}
      }
    }
  
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
  //      float dist_cm = ultrasonic.Ranging(1);       // get distance
  // Serial.println(dist_cm);
  // delay(500);
  //Микрофон
if ((TEMP!=analogRead(micA)) and (digitalRead(micD) == HIGH)){
  TEMP = analogRead(micA);
  Serial.println(TEMP); // выводим в порт
}

// if (TEMP!=analogRead(micA)){
//   TEMP = analogRead(micA);
//   Serial.println(TEMP); // выводим в порт
// }



  //Кнопка для колонок, если 1, то сменить на выходе сигнал
  /*if (digitalRead(btnSpeac) == HIGH) {
    digitalWrite(speacRelay, !digitalRead(speacRelay));
    delay(40);
    while (digitalRead(btnSpeac) == HIGH) //Ждать пока не отжмется кнопка
    {
      delay(40);
    }
  }*/
//Принять тнформацию по сериал
  //Получаем строку в переменную
  
  
  /*while (Serial.available() > 0)
  {
    ch = Serial.read();
    str += ch;
    //Serial.println("//"+ch);
  }*/
  /*str.trim();
  if (str == "on")
  {
    digitalWrite(speacRelay, LOW);
  }
  if (str == "off") {               
    digitalWrite(speacRelay, HIGH); //    то выключаем светодиод
  }
  if (str != "")
  {
    Serial.println("/"+str+"/");
    str = ""; //Сброс переменной
    delay(500);
  }*/
  //**********************************************
  /*sensorValue = analogRead(analogInPin);
    Serial.print("Microphone value= ");
    Serial.println(sensorValue); */

  //int sensorValue = analogRead(analogInPin) * 5.0 / 1024.0 * 1000.0; // значения в милливольтах
    


 


}
//Вывод часов
void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
  delay(1000);
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
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
}*/
//void setColor(int red, int green, int blue)
void setColor(int i)

{
  analogWrite(redPin, color[i][0]);
  analogWrite(greenPin, color[i][1]);
  analogWrite(bluePin, color[i][2]);

  
  Serial.println(String(i,DEC));
  Serial.println(String(color[i][0], DEC) + " "+String(color[i][1], DEC)+" "+String(color[i][2], DEC));
  Serial.println(String(collorRow+99, DEC));
  Serial.println(" ");
  
  
  /*analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);*/
}

void readColor ()
{
  if (Serial.available() > 0) {  //если есть доступные данные
        // считываем байт
    str="";
    redNum = "";
    greenNum ="";
    redNum = "";
        str = Serial.read();
    redNum = str.substring(1,3);
    greenNum = str.substring(3,6);
    redNum = str.substring(6,8);
    
    /*Serial.println(redNum);
    Serial.println(greenNum);
    Serial.println(redNum);*/
    //String stringOne =  String(color[1][1], DEC);
    Serial.println(String(color[1][1], DEC)); // Вязть данные из двумерного массива и конвертировать в строку
    //Serial.println(String((color[1][1])byteArray));
    
    
    
  }

}



