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
//#include <avr/pgmspace.h> // ДлЯ Progem запись во флэш

// #include "read_serial.h"

// Прототипы функций

void sendToMQTT ();

// Устанавливаем значения в регистры 0-номер регистра, 1-значение,2-сообщение
void set_reg_shift(byte num, byte val, bool send);

// получить значения регистров  0 - № PCF (array_name), 1 - № регистра PCF
byte get_value_regs(byte i, byte j);

// Чтение и установка данных из Serial
void read_serial();

// чтение регистров  и запись в active_reg
void read_array_regs ();

// очищаем переменные Serial
void serial_clear();


#include "my_lib.h"


//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x21,16,2);  

//КОНСТАНТЫ
#define DHTPIN 6
#define DHTTYPE           DHT11
//#define btnSpeac      2
#define BTN      9
//#define micD      7
#define sr04Trig  10
#define sr04Echo  11
//Ultrasonic ultrasonic(sr04Trig, sr04Echo);

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
   

byte TempBtn=0;
int tempBtns;
int tempTherm;
int sensorValue = 0;
int val;
int myInts[10];
int counter = 0;
int counterSr04 = 0;
char ch = 0;

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


//*******************************************
//ПЕРЕМЕННЫЕ ДЛЯ РЕГИСТРОВ
#define ARRAY_LENGTH 3

//Названия сдвиговых регистров
const char *array_name[]  = 
{
  "/h/shift",   // 0
  "/k/shift",     // 1
  "/h/dht11t"     // 2
};


// Количество портов в регистрах
const byte array_count[ARRAY_LENGTH] =
{
	8,
	8,
	1
};


// 0 - имя массива. 1 - номер регистра
byte active_reg[2] =
{
	255,
	255
};


	//переменные 74HC595

// массив значение регистров 2 шт 8 бит
byte registers_shifts[2] =
{
	0b00000000,
	0b00000000
};

//byte registers_shift_1 = 0b00000000;
//byte registers_shift_2 = 0b00000000;

// временное значение сдвигового регичтра
byte value_reg_shift = 0;

//PCF8574
byte num_reg_PCF8574 = 0;
byte value_reg_PCF8574 = 0;
byte value_reg = 0;

#include <PCF8574.h>  // Расширитель портов
PCF8574 expander(0x20);  
PCF8574 expander_2(0x21); 

//переменные для работы с сериал портом
byte id = 0;
//byte check = 0;
byte topic = 0;
byte num = 255;
byte value = 0;

String serial_temp = "";
byte serial_num = 0;

String arduino_id = "1";

char serial_char = 0;

struct serial_struct
{
	String id = "";
	String topic = "";
	String num = "";
	String value = "";

	byte b_id = 0;
	byte b_topic = 0;
	byte b_num = 0;
	byte b_value = 0;
};

serial_struct serial_vars;






void setup()  
{
	//LCD
	/*lcd.init();                     
	lcd.backlight();// Включаем подсветку дисплея
	lcd.print("PUSH?");*/
	//Конец LCD
	pinMode(sr04Trig, OUTPUT); 
	pinMode(sr04Echo, INPUT); 
	//Serial.begin(115200);
	Serial.begin(115200);
	Serial.println("Start");

	//Расширитель портов
	expander.begin();
	/*expander.pinMode(0, INPUT);
	expander.pullDown(0);
	expander.pinMode(1, INPUT);
	expander.pullDown(1);
	expander.pinMode(2, INPUT);
	expander.pullDown(3);*/

	expander_2.begin();
	/*expander_2.pinMode(0, INPUT);
	expander_2.pullDown(0);
	expander_2.pinMode(1, INPUT);
	expander_2.pullDown(1);
	expander_2.pinMode(2, INPUT);
	expander_2.pullDown(3);*/

	//74HC595
	//устанавливаем режим OUTPUT 
	pinMode(ST_CP, OUTPUT);
	pinMode(SH_CP, OUTPUT);
	pinMode(DS, OUTPUT);



	//pinMode(btnSpeac, INPUT);   //а 2й – входом
	//pinMode(btn, INPUT);   //а 3й – входом
	pinMode(BTN, INPUT);   //а 9й – входом кнопкиъ

	pinMode(speacRelay, OUTPUT);  //8й вывод будет выходом
	digitalWrite(speacRelay, LOW);

	//pinMode(micLamp, OUTPUT);  //9й вывод будет выходом
	// digitalWrite(micLamp, LOW);


	// время
	while (!Serial) ; // Только для платы Leonardo
	setSyncProvider(RTC.get);   // получаем время с RTC
	if (timeStatus() != timeSet)
		Serial.println("Unable to sync with the RTC"); //синхронизация не удаласть
	else
		Serial.println("RTC has set the system time");

	dht.begin();

	//void read_serial();

}

void loop()
{
	 //num_reg_PCF8574 = read_PCF8574(0, expander);
	 //num_reg_PCF8574 = read_PCF8574(1, expander_2);

	//delay(300);
	//Serial.println("test" + String(active_reg[0]));

	// проверяем все регистры и получаем данные. если что-то нажато
	read_array_regs();

	// читаем данные с Serial порта и записываем в структуру
	read_serial();

	// проверяем установленный регистр и устанавливаем сдвиговый
	if (active_reg[1] != 255)
	{
		// считываем бит, который установлен и инвертируем его,
		// далее записываем его в сдвиговый регистр

		value_reg_shift = bitRead(registers_shifts[active_reg[0]], active_reg[1]);
		value_reg_shift = !value_reg_shift;
		set_reg_shift(active_reg[1], value_reg_shift, 1);

		Serial.println("i"+ arduino_id 
		+ 't' + String(active_reg[0]) 
		+ 'n' + String(active_reg[1]) 
		+ 'v' + String(value_reg_shift));

		active_reg[0] = 255;
		active_reg[1] = 255;
	}

	// sendToMQTT();
	delay(1);
	float dist_cm=120;
	if (counterSr04 == 0)
	{
		/*dist_cm = ultrasonic.Ranging(1);       // get distance
		delay(100);
		if (dist_cm < 20)
		{
			dist_cm = ultrasonic.Ranging(1);
		}*/
	}
	//Serial.println(dist_cm);
	// Проверяем условия по включению светодиода
	if (digitalRead(BTN)==HIGH and dist_cm<20)
	{
		delay(50);
		TempBtn=!TempBtn;
		set_reg_shift(1, TempBtn, 1);
		digitalWrite(speacRelay, HIGH);
		Serial.println("/hall/lamp@"+String(TempBtn)+"%");	
		Serial.println(String(dist_cm));
		while(digitalRead(BTN) == HIGH || dist_cm < 20)
		{
			//dist_cm = ultrasonic.Ranging(1);
			delay(5);
		}
		delay(40);
		counterSr04=2000;
	}

	// Делаем задержку на передачу дистанции, примерно в 2 секунды
	if (counterSr04!=0)
	{
		counterSr04--;
	}
	
	//Вывод данных 5000 - 20 минут
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
		if (isnan(h) || isnan(t)) 
		{
			Serial.println("/hall/dht11@off%");
			return;
		} 

		Serial.println("/hall/dht11t@"+String(t)+"%");
		Serial.println("/hall/dht11h@"+String(h)+"%"); 
		

		
	}
	counter++;

	//Serial.println("df");
	//delay(200);	
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
	  if (strAdr == "/hall/light")
	  digitalWrite(speacRelay, !strVal.toInt()); //
 
	//Сдвиговый регистр	
	String tempStrAdr = strAdr.substring(0, strAdr.length()-1);
	
	if (tempStrAdr == "/hall/shiftReg")
	{
		String lastStrAdr = strAdr.substring(strAdr.length()-1);
		set_reg_shift(lastStrAdr.toInt(), strVal.toInt(), 1);
	}
	
	if (strAdr=="/hall/lamp"){
		set_reg_shift(1, TempBtn, 1);
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

// чтение регистров  и запись в active_reg
void read_array_regs ()
{
	// проверяем активный номер регистра
	if (active_reg[0] == 255 and active_reg[1] == 255)
		for (byte i=0; i < ARRAY_LENGTH; i++)
		{
			value_reg = 0;
			for (byte j=0; j < array_count[i]; j++)
			{	
				// если количество регистров = 1, то вернется 0 и ничего не случится
				// если это расширитель регистров, а там стоит 8, то заходим в цикл и ждем
				// пока отпустится кнопка, что бы не было нескольких срабатыванй

				//Serial.println("1 - " + String(i) + "-" + String(j));
				value_reg = get_value_regs(i, j);
				while(value_reg > 0 )
				{
					//Serial.println("2 - " + String(value_reg));
					//delay(500);
					value_reg = get_value_regs(i, j);

					//если отпустили кнопку, то устанавливаем регистры
					if (value_reg == 0)
					{
						active_reg[0] = i;  // номер имени

						//Если это расширитель, то в 1 записываем номер нажатого регистра
						if (array_count[i] > 1)
							active_reg[1] = j;
						// иначе, пишем 255, что означает обычную кнопку к контроллеру
						else
							active_reg[1] = 255;
					}

				}

		  	}
		}

}

// получить значения регистров  0 - № PCF (array_name), 1 - № регистра PCF
byte get_value_regs(byte i, byte j)
{
	switch (i) 
	{
		case 0:
			return expander.readButton(j);
			break;
		case 1:
			return expander_2.readButton(j);
			break;
		default:
			return 0;
	}
}

//Установить бит в сдвиговый регистр 0- 
void set_reg_shift(byte num, byte val, bool send)
{
	bitWrite(registers_shifts[active_reg[0]], num,val);

	//Сдвиг через сдвиговый регистр 74HC595
	digitalWrite(ST_CP, LOW);
	shiftOut(DS, SH_CP, MSBFIRST, registers_shifts[1]);
    shiftOut(DS, SH_CP, MSBFIRST, registers_shifts[0]);
    digitalWrite(ST_CP, HIGH);

	if (send)
		Serial.println("0 - " + String(registers_shifts[0]) +"1 - " + String(registers_shifts[1]) );	

}


void read_serial()
{

	while (Serial.available() > 0)
	{
		//delay(1);
		// забираем первый символ
		serial_char = Serial.read();

	 	switch (serial_char) 
		{
			case 'i':
				serial_num = 1;
				continue;

			case 't':
				// если id сообщения равен устройству, то записываем следующее значение
				if (serial_vars.id == arduino_id)
				{
					serial_num = 2;
				}	
				// иначе, мы обнуляем переменные и очищаем кэш сериала
				else
				{
					serial_clear();

					while (Serial.available() > 0)
					{
						Serial.read();	
					}
				}	
				continue;

			case 'n':
				serial_num = 3;
				continue;

			case 'v':
				serial_num = 4;
				continue;
		}

		// ессли получаем не число, то сбрасываем переменные и буфер
		if (!isdigit(serial_char))
		{
			serial_clear();

			while (Serial.available() > 0)
			{
				Serial.read();	
			}

		}

		switch (serial_num) 
		{
			case 1:
				serial_vars.id += serial_char;
				break;
			case 2:
				serial_vars.topic += serial_char;
				break;
			case 3:
				serial_vars.num += serial_char;
				break;
			case 4:
				serial_vars.value += serial_char;
				break;
		}
	}

	// После проверки буфера сообщений проверяем id и если он есть, то ковертируем в цифры
	if (serial_vars.id != "")
	{
		serial_vars.b_id = serial_vars.id.toInt();
		serial_vars.b_topic = serial_vars.topic.toInt();
		serial_vars.b_num = serial_vars.num.toInt();
		serial_vars.b_value = serial_vars.value.toInt();

		Serial.println(serial_vars.topic + "-" + serial_vars.num + "-" + serial_vars.value);
		
		active_reg[0] = serial_vars.b_topic;
		active_reg[1] = serial_vars.b_num;

		set_reg_shift(serial_vars.b_num, serial_vars.b_value, 0);
		serial_clear();
	}




}


void serial_clear()
{
	serial_vars.id = "";
	serial_vars.topic = "";
	serial_vars.num = "";
	serial_vars.value = "";

	serial_vars.b_id = 0;
	serial_vars.b_topic = 0;
	serial_vars.b_num = 0;
	serial_vars.b_value = 0;

	serial_num = 0;

	active_reg[0] = 255;
	active_reg[1] = 255;

}
// i1t0n2v1    i1t0n2v0

