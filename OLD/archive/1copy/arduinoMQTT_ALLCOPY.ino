/*
   TimeRTC.pde
   example code illustrating Time library with Real Time Clock.
   pwm -  5, 6 : 9, 10 : 11  13
   
	Timer0 - используется millis (), micros (), delay () и ШИМ на пинах 5 и 6
	Timer1 - используется для Servo, библиотеки WaveHC и ШИМ на пинах 9 и 10
	Timer2 - используется Tone и ШИМ на пинах 11 и 13

*/
//#include <Ultrasonic.h>  //Дальнометр SR04
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <PCF8574.h>
#include "IRremote.h"
#include <DHT.h>
//#include <avr/pgmspace.h> // ДлЯ Progem запись во флэш

#include <Time.h>


//КОНСТАНТЫ
#define DHTPIN 6
#define DHTTYPE           DHT11
//#define btnSpeac      2
#define btn      9
//#define micD      7
//#define sr04Trig  5
//#define sr04Echo  

//74HC595 
int DS = 2;
int ST_CP = 3;
int SH_CP = 4;
#define speacRelay      8
//#define micLamp      3

//Инфрокарсный пульт
IRrecv irrecv(7);
decode_results results;
//RGB светодиод
#define redPin  10
#define greenPin  13
//#define bluePin  9



const int btns = A0;   //496, 333, ,249, 199, 165
const int sensTherm = A2; 
const int sensLight = A3;  
   

int TempBtn=0;
int tempBtns;
int tempTherm;
int sensorValue = 0;
int val;
int myInts[10];
int counter = 0;
char ch = 0;
byte registers = 0b00000000;
String str = "";
byte  strKey = 0;
String strAdr = " ";
String strVal = " ";
 //Ultrasonic ultrasonic(sr04Trig, sr04Echo);
 


//Драйвер двигателя
/*int IN1 = 5; 
int IN2 = 6;
int EN1 = 9;*/



/*byte path[3]= {
  B01111111,
  B10111111,
  B11011111
  };*/
  
  //Rgb светодиод
/*const int RedPin = 1;
const int GreenPin = 2;
const int BluePin = 3;
int redPin = 5;
int greenPin = 6;
int bluePin = 9;*/

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
	{248, 23, 62}
  };
  int collorRow = sizeof(color) /3; //Кол-во строк массива, 3 столбца 
bool isTimeSet = false; //флаг, указывающий на то, была ли уже задана дата


int tp1,tp2,tp3 = 0;
byte varIrc = 0;


DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

//PCF8574 expander;
void setup()  {
	 Serial.begin(115200);
  Serial.println("Start");
	//Расширитель портов
	/*expander.begin(0x20);
	expander.pinMode(4, INPUT);
	expander.pullDown(4);*/
	//pinMode(btnSpeac, INPUT);   //а 2й – входом
  //pinMode(btn, INPUT);   //а 3й – входом
  pinMode(btn, INPUT);   //а 9й – входом кнопкиъ

  pinMode(speacRelay, OUTPUT);  //8й вывод будет выходом
  digitalWrite(speacRelay, LOW);

  //pinMode(micLamp, OUTPUT);  //9й вывод будет выходом
 // digitalWrite(micLamp, LOW);
  

  
  //устанавливаем режим OUTPUT 74HC595
  
  pinMode(ST_CP, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(DS, OUTPUT);
  
  //драйвер мотора
  /*pinMode (IN2, OUTPUT);
  pinMode (IN1, OUTPUT);
  pinMode (EN1, OUTPUT);*/
  
  //RGB
	/*pinMode(redPin, OUTPUT);
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);*/



 

 
  //while (!Serial) ; // wait until Arduino Serial Monitor opens
  /*setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");*/

//Инфрокрасный пульт
//irrecv.enableIRIn();

// время
  while (!Serial) ; // Только для платы Leonardo
  setSyncProvider(RTC.get);   // получаем время с RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC"); //синхронизация не удаласть
  else
    Serial.println("RTC has set the system time");

dht.begin();

}

void loop()
{
	//expand();

	//Сдвиговый регистр
	/**/
	

	//Хлопок
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
 //if (TEMP!=analogRead(micA)){ //Для датчика газа
/*//if ((TEMP!=analogRead(micA)) and (digitalRead(micD) == HIGH)){
  TEMP = analogRead(micA);
  Serial.println(TEMP); // выводим в порт
}*/


// if (TEMP!=analogRead(micA))
	/*{
	   TEMP = analogRead(micA);
	   Serial.println(TEMP); // выводим в порт
 	}*/
	
  //if (digitalRead(btn) == HIGH) {
	  
	 /* tempBtns = analogRead(btns);  // обязательно в переменную, иначе не рабоатет
	  //Serial.println(analogRead(btns));
	  if (tempBtns>=230)   //функция проверок от ошибок, при нажатии значения плавают от меньшего
	  {	  
		delay(40);
		tempBtns = analogRead(btns);
	  }
	if (tempBtns <=350 and tempBtns >230)
	{ 
	
		setRegShift(1); 
	}
	*/
  //Кнопка для колонок, если 1, то сменить на выходе сигнал
  //if (digitalRead(btnSpeac) == HIGH) 
	//if (tempBtns <=460 and tempBtns >350)  
  //{
	 
	   
	  /*
    digitalWrite(speacRelay, !digitalRead(speacRelay));
	if (digitalRead(speacRelay))
		Serial.println("/home/hall/ard1relay9@0%");
	else
		Serial.println("/home/hall/ard1relay9@1%");
	Serial.println(analogRead(btns));
    delay(400);  */
	
 // }
  
  //Для реле 1
  	//if (tempBtns <=595 and tempBtns >460)  
  //{
	
		//setColor(tp);
		//Serial.println(analogRead(btns));
		
	  
   //setRegShift(3);  
   //Для движка
   /*
 digitalWrite (IN2, HIGH);
  digitalWrite (IN1, LOW); 
  analogWrite(EN1,55);
  delay(2000);
  analogWrite(EN1,105);
  delay(2000);
  analogWrite(EN1,255);
  delay(2000);
  analogWrite(EN1,0);
  delay(5000);*/
  //}

  
  
//Принять тнформацию по сериал
  //Получаем строку в переменную
  
  /*while (Serial.available() > 0)
  {
	  delay(2);
    ch = Serial.read();
    str += ch;
  }
  str.trim();
  if (str == "on")
  {
    digitalWrite(speacRelay, LOW);
  } 
  if (str == "off") {               
    digitalWrite(speacRelay, HIGH); //    то выключаем светодиод
  }
  
	if (str == "time") {               
    digitalClockDisplay();
  }

  if (str != "")
  {
    //Serial.println("/"+str+"/");
    str = ""; //Сброс переменной
    delay(500);
  }*/
  //**********************************************
  /*sensorValue = analogRead(analogInPin);
    Serial.print("Microphone value= ");
    Serial.println(sensorValue); */

  //int sensorValue = analogRead(analogInPin) * 5.0 / 1024.0 * 1000.0; // значения в милливольтах
    
//Настрйока времени
//digitalClockDisplay();

//Инфракрасный пульт
 /*if (irrecv.decode(&results)) 
  {
	 
    int res = results.value;
	Serial.println(res, HEX);
	
	//RGB светодиод
		
	if (res == 0x4EB1)
	{
		if (tp1<255)
			tp1++;
		else
			tp1=0;
		analogWrite(redPin, tp1);
		Serial.println(String(tp1,DEC));
		varIrc =1;
		setRegShift(1); 
	}
		
	if (res == 0xFFFF8E71 )  
	{
		//for (int t = 0; t < collorRow; t++) //цикл на перебор строк массива
		if (tp2<255)
			tp2++;
		else
			tp2=0;
		analogWrite(greenPin, tp2);
		Serial.println(String(tp2,DEC));
		varIrc =2;
		setRegShift(2); 
	}
	  
	if (res == 0xFFFFC639 )  
	{
		//for (int t = 0; t < collorRow; t++) //цикл на перебор строк массива
		if (tp3<255)
			tp3++;
		else
			tp3=0;
		analogWrite(bluePin, tp3);
		Serial.println(String(tp3,DEC));
		varIrc =3;
		setRegShift(3); 
	}
	
	//ОТРИЦАНИЕ
	if (res == 0x4FB)
	{
		if (tp1>0)
			tp1--;
		else
			tp1=255;
		analogWrite(redPin, tp1);
		Serial.println(String(tp1,DEC));
		varIrc =4;
	}
		
	if (res == 0xFFFF847B )  
	{
		//for (int t = 0; t < collorRow; t--) //цикл на перебор строк массива
		if (tp2>0)
			tp2--;
		else
			tp2=255;
		analogWrite(greenPin, tp2);
		Serial.println(String(tp2,DEC));
		varIrc =5;
	}
	  
	if (res == 0xFFFF9C63 )  
	{
		//for (int t = 0; t < collorRow; t--) //цикл на перебор строк массива
		if (tp3>0)
			tp3--;
		else
			tp3=255;
		analogWrite(bluePin, tp3);
		Serial.println(String(tp3,DEC));
		varIrc =6;
	}
	
	if (res == 0xFFFFFFFF)
	{

		if (varIrc == 1)
		{
			if (tp1<=255)
				tp1++;
			else
				tp1=0;
			analogWrite(redPin, tp1);
			Serial.println(String(tp1,DEC));
		}
		 if (varIrc == 2)
		{
			if (tp2<255)
				tp2++;
			else
				tp2=0;
			analogWrite(greenPin, tp2);
			Serial.println(String(tp2,DEC));
		}
		 if (varIrc == 3)
		{
			if (tp3<255)
				tp3++;
			else
				tp3=0;
			analogWrite(bluePin, tp3);
			Serial.println(String(tp3,DEC));
		}
		if (varIrc == 4)
		{
			if (tp1>0)
				tp1--;
			else
				tp1=255;
			analogWrite(redPin, tp1);
			Serial.println(String(tp1,DEC));
		}
		 if (varIrc == 5)
		{
			if (tp2>0)
				tp2--;
			else
				tp2=255;
			analogWrite(greenPin, tp2);
			Serial.println(String(tp2,DEC));
		}
		 if (varIrc == 6)
		{
			if (tp3>0)
				tp3--;
			else
				tp3=255;
			analogWrite(bluePin, tp3);
			Serial.println(String(tp3,DEC));
		}
	}
	

	
	//Сброс светодиодов
	 if (res == 0x14EB )  
	{
		tp1 = 0;
		tp2 = 0;
		tp3 = 0;

		analogWrite(redPin, tp1);
		analogWrite(greenPin, tp1);
		analogWrite(bluePin, tp1);
		varIrc =0; 
	}
	
	
	
	irrecv.resume();
  }*/

	//checkTime;
	//Serial.print("Termister: ");
	//Serial.print(float(Thermister(analogRead(2))), 1);
	sendToMQTT ();
	delay(1);
	if (btn==1){
		delay(50);
		TempBtn=!TempBtn;
		setRegShift(1, TempBtn);
		Serial.println("/hall/lamp@"+String(TempBtn)+"%");	
		Serial.println("fdfdgdfgdfhg");
	}
	//Вывод данных
if (counter == 5000)
{	
	counter = 0;
	Serial.println("/hall/sensTherm@"+String(float(Thermister(analogRead(sensTherm))),1)+"%"); 
	Serial.println("/hall/sensLight@"+String(float(analogRead(sensLight)),1)+"%");
	 Serial.println("/hall/timeEsp@"+digitalClockDisplay()+"%");
	delay(5);
	 val = !digitalRead(speacRelay);

	  //Считываем влажность и температуру
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	
	// Проверка удачно прошло ли считывание влажности и температуры
	if (isnan(h) || isnan(t)) {
		Serial.println("/hall/dht11@off%");
	return;
	} 
	 Serial.println("/hall/dht11t@"+String(t)+"%");
	 Serial.println("/hall/dht11h@"+String(h)+"%"); 
	

	
}
counter++;
}

//*********************************************************************************************
//***************************************ФУНКЦИИ***********************************************
//*********************************************************************************************
/*void expand()
{
	for (int i=0; i<7; i++)
	{
		byte value = expander.digitalRead(i);
		if (value == 0)
		{
				while (expander.digitalRead(i) != 0)
			{

			}
			setRegShift(i, !bitRead(registers, i));
			
			Serial.println("Port-"+String(i)+" = ");
			Serial.println(value, DEC);
			
		}
	}
	
}*/

void sendToMQTT ()
{
	 while (Serial.available() > 0)
  {
	delay(1);
    ch = Serial.read();
	if (ch == '@')
	{
		strKey = 1;
		continue;
	}
	if (ch == '%')
	{
		strKey = 1;
		break;
	}
		
	if (strKey == 0)
		strAdr += ch;
	else
		strVal += ch;
  }
  strAdr.trim();
  strVal.trim();
  //Если получены какие-то данные с ЕСП
  if (strAdr != "" || strVal != "" )
  {
	  //Serial.println(strAdr+strVal);
	  if (strAdr=="/hall/light")
	  digitalWrite(speacRelay, !strVal.toInt()); //
 
	//Сдвиговый регистр	
	String tempStrAdr = strAdr.substring(0, strAdr.length()-1);
	
	if (tempStrAdr = "/hall/shiftReg")
	{
		String lastStrAdr = strAdr.substring(strAdr.length()-1);
		setRegShift(lastStrAdr.toInt(), strVal.toInt());
	}
	/*char strAdrBuf[25];
	char strValBuf[10];
	strAdr.toCharArray(strAdrBuf, 25); 
	strVal.toCharArray(strValBuf, 10); 
    client.publish(strAdrBuf, strValBuf);*/
    strAdr = " "; //Сброс переменных
	strVal = " ";
	strKey = 0;
  }
}
//Термистор
double Thermister(int RawADC) {
	double Temp;
	Temp = log(((10240000/RawADC) - 10000));
	Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
	Temp = Temp - 273.15;		// Kelvin to Celcius
	//Temp = (Temp * 9.0)/ 5.0 + 32.0;		// 1 способ Convert Celcius to Fahrenheit 
	//Temp = (Temp * 1.8) + 32.0;		// 2 способ Convert Celcius to Fahrenheit
	return Temp;
}
//Вывод часов
String digitalClockDisplay() {
	String clockTemp = "";
  // digital clock display of the time
  /*Serial.print(hour());
  //printDigits(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();*/
  //delay(1000);
  return clockTemp = String(hour())+printDigits(minute())+printDigits(second());
}

String printDigits(int num) {
	String a =":";
  // utility function for digital clock display: prints preceding colon and leading 0
  //Serial.print(":");
  if (num < 10)
    //Serial.print('0');
	a+="0";
	 return a+=String(num);
 // Serial.print(num);
}
/*void convertDigits(String digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}*/
//Установить бит в сдвиговый регистр
void setRegShift(int num, int val)
{
	bitWrite(registers, num,val);
	regShift(registers);
	
	/*if (bitRead(registers,num))
	{
		bitWrite(registers, num,0);
		regShift(registers);
		Serial.println("/hall/RegShift"+String(num)+"@1%");
	}
	else
	{
		bitWrite(registers, num,1);
		regShift(registers);
		Serial.println("/hall/RegShift"+String(num)+"@0%");
	}*/
	
	//Serial.println(analogRead(btns));
    //delay(400);

}
//Сдвиг через сдвиговый регистр 74HC595
void regShift (byte a){
	digitalWrite(ST_CP, LOW);
    shiftOut(DS, SH_CP, MSBFIRST, a);
    digitalWrite(ST_CP, HIGH);
	//Serial.print("------");
	//Serial.println(a);
}



























//void setColor(int red, int green, int blue)

/*void setColor(int i)

{
	analogWrite(redPin, color[i][0]);
	analogWrite(greenPin, color[i][1]);
	analogWrite(bluePin, color[i][2]);

	
	Serial.println(String(i,DEC));
	Serial.println(String(color[i][0], DEC) + " "+String(color[i][1], DEC)+" "+String(color[i][2], DEC));
	Serial.println(String(collorRow+99, DEC));
	Serial.println(" ");
	
}*/

/*void readColor ()
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
		
		//Serial.println(redNum);
		//Serial.println(greenNum);
		//Serial.println(redNum);
		//String stringOne =  String(color[1][1], DEC);
		//Serial.println(String(color[1][1], DEC)); // Вязть данные из двумерного массива и конвертировать в строку
		//Serial.println(String((color[1][1])byteArray));	
	}

}*/


void checkTime ()
{
	  if (timeStatus() == timeSet) 
  {
    /*if (digitalRead(btn) == HIGH) 
	{
      //digitalClockDisplay();
    }*/
    if (hour() == 6 and minute() == 45 and digitalRead(5) == 1) 
	{
      digitalWrite(speacRelay, LOW);
    }

  } 
  else 
  {
    Serial.println("The time has not been set.  Please run the Time");
    Serial.println("TimeRTCSet example, or DS1307RTC SetTime example.");
    Serial.println();
    delay(4000);
  }
}




//Работы с цифровым сигналом, кнопка
	 //digitalWrite(relay1, !digitalRead(relay1));
	/*if (digitalRead(relay1))
		Serial.println("/home/hall/ard1relay1@1%");
	else
		Serial.println("/home/hall/ard1relay1@0%");*/
   // while (digitalRead(btnSpeac) == HIGH) //Ждать пока не отжмется кнопка


   
   
   
   
   
   
   


void setTimeFromFormatString(String time)
{
  //ДД.ММ.ГГ чч:мм:сс
  int day = time.substring(0, 2).toInt();
  int month = time.substring(3, 5).toInt();
  int year = time.substring(6, 8).toInt();
  int hours = time.substring(9, 11).toInt();
  int minutes = time.substring(12, 14).toInt();
  int seconds = time.substring(15, 17).toInt();
  TimeElements te;
  te.Second = seconds;
  te.Minute = minutes;
  te.Hour = hours;
  te.Day = day;
  te.Month = month;
  te.Year = year + 30; //год в библиотеке отсчитывается с 1970. Мы хотим с 2000
  time_t timeVal = makeTime(te);
  RTC.set(timeVal);
  setTime(timeVal);
}

