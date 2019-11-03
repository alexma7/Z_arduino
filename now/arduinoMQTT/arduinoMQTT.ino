/*
   TimeRTC.pde
   example code illustrating Time library with Real Time Clock.
   pwm -  5, 6 : 9, 10 : 11  13
   
	Timer0 - используется millis (), micros (), delay () и ШИМ на пинах 5 и 6
	Timer1 - используется для Servo, библиотеки WaveHC и ШИМ на пинах 9 и 10
	Timer2 - используется Tone и ШИМ на пинах 11 и 13

*/
#include <Ultrasonic.h>  //Дальнометр SR04
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <PCF8574.h>
#include "IRremote.h"
#include <DHT.h>
//#include <avr/pgmspace.h> // ДлЯ Progem запись во флэш
#include <Time.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x21,16,2);  

//КОНСТАНТЫ
#define DHTPIN 6
#define DHTTYPE           DHT11
//#define btnSpeac      2
#define btn      9
//#define micD      7
#define sr04Trig  10
#define sr04Echo  11
Ultrasonic ultrasonic(sr04Trig, sr04Echo);

//74HC595 
#define DS  2
#define ST_CP  3
#define SH_CP  4

#define speacRelay      8
//#define micLamp      3

//Инфрокарсный пульт
IRrecv irrecv(7);
decode_results results;
//RGB светодиод
//#define redPin  10
//#define greenPin  13
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
int counterSr04 = 0;
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
//LCD
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.print("PUSH?");
//Конец LCD
	pinMode(sr04Trig, OUTPUT); 
  pinMode(sr04Echo, INPUT); 
	 //Serial.begin(115200);
   Serial.begin(9600);
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

	sendToMQTT ();
	delay(1);
	float dist_cm=120;
	if (counterSr04==0){
		 dist_cm = ultrasonic.Ranging(1);       // get distance
	}
	//Serial.println(dist_cm);
	if (digitalRead(btn)==HIGH || dist_cm<20){
		delay(50);
		TempBtn=!TempBtn;
		setRegShift(1, TempBtn);
		Serial.println("/hall/lamp@"+String(TempBtn)+"%");	
		while(digitalRead(btn)==HIGH || dist_cm<20){
			dist_cm = ultrasonic.Ranging(1);
			delay(5);
		}
		delay(40);
		counterSr04=2000;
	}
	if (counterSr04!=0){
		counterSr04--;
	}
	
	//Вывод данных
if (counter == 5000)
{	
	counter = 0;
	Serial.println("/hall/sensTherm@"+String(float(Thermister(analogRead(sensTherm))),1)+"%"); 
	Serial.println("/hall/sensLight@"+String(float(analogRead(sensLight)),1)+"%");
	 Serial.println("/hall/timeEsp@"+digitalClockDisplay()+"%");
	//delay(5);
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
	
	if (strAdr=="/hall/lamp"){
		setRegShift(1, TempBtn);
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

