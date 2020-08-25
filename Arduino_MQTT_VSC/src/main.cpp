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

// Устанавливаем значения в регистры  0 - topic № сдвиговика в массиве , 0- № регистра, 1-значение,2-сообщение (отправлять или нет) //Уст бит в сдвиговый регистр 0 - topic, 1 - num, 2 - val, 3 - send отправлять или нет 
//void set_reg_shift(byte topic, byte num, byte val, bool send);
void set_reg_shift(bool send);

// получить значения регистров  0 - № PCF (array_name), 1 - № регистра PCF
byte get_value_regs_PCF(byte i, byte j);

// Чтение и установка данных из Serial
void read_serial();

// чтение регистров  и запись в active_reg
void read_array_regs ();

// проверка и установка сдвиговых регистров из нажатых PCF
void set_push_PCF_to_shiftreg ();

// очищаем переменные Serial
void serial_clear();

// Отправляем сообщение в Serial
void send_to_serial( byte topic, byte num, int value);


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
//Кол-во сдвиговых регистров
#define COUNT_SHIFT_REG  2

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

	//переменные 74HC595

byte array_shift_reg[] =
{
	0b00000000,
	0b00000000
};
byte count_reg_shift = sizeof(array_shift_reg); //Кол-во строк массива, 3 столбца 

 byte array_count_reg[][3]  = {
//	PCF			>3s			связь со сдвиговым
	{0b00000000, 0b00000000, 0b11000011},
	{0b00000000, 0b00000000, 0b11000011}
  };

byte count_reg_PCF = sizeof(array_count_reg)/3; //Кол-во строк массива, 3 столбца 

//Для циклов
byte i, j;
boolean bit_PCF_reg;
byte byte_PCF_reg;
//byte registers_shift_1 = 0b00000000;
//byte registers_shift_2 = 0b00000000;

// временное значение сдвигового регичтра
byte value_reg_shift = 0;

//PCF8574
byte num_reg_PCF8574 = 0;
byte value_reg_PCF = 0;

byte value_time_delay = 0;

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



// Объявление переменных
// для read_serial
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

//Структура для хранения данных из сериала
serial_struct serial_vars;
byte serial_num = 0;
String arduino_id = "1";






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

	// TEST
	Serial.println("count_reg_shift - " + String(count_reg_shift));
	Serial.println("count_reg_PCF - " + String(count_reg_PCF));
}

// TODO:  Основной цикл
void loop()
{

	//Serial.println("START");
	// читаем данные с Serial порта и записываем в структуру
	read_serial();

	// проверяем все регистры и получаем данные. если что-то нажато
	read_array_regs();

	// Устанавливаем нажатые кнопки в сдвиговые регистры
	set_push_PCF_to_shiftreg();

	//  TODO: Установить значения в сдвиговые регистры
	set_reg_shift(0);

	serial_clear();


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
	//Serial.println("END");
	//Serial.println("df");
	//delay(200);	
}

//*********************************************************************************************
//***************************************ФУНКЦИИ***********************************************
//*********************************************************************************************


// TODO: чтение регистров  и запись в active_reg
void read_array_regs ()
{
	//Serial.println("Start arr");
	for (i=0; i < count_reg_PCF; i++)
	{
			value_reg_PCF = 0;
			

			// Перебираем регистры их 8 штук
			for (j=0; j < 8; j++)
			{	
				value_time_delay = 0;
				// пока отпустится кнопка, что бы не было нескольких срабатыванй

				//Serial.println("1 - " + String(i) + "-" + String(j));
				value_reg_PCF = get_value_regs_PCF(i, j);
				while(value_reg_PCF > 0 )
				{
					/*// Отладка
					Serial.println("value_reg_PCF - " + String(value_reg_PCF));
					Serial.println("i - " + String(i));
					Serial.println("j - " + String(j));
					*/
					//Serial.println("2 - " + String(value_reg_PCF));
					//delay(500);
					value_reg_PCF = get_value_regs_PCF(i, j);

					//если отпустили кнопку, то устанавливаем регистры
					if (value_reg_PCF == 0)
					{
						
						//Установка в регистр, нажатия кнопки
						bitSet(array_count_reg[i][0], j);
						if(value_time_delay == 255)
						{
							bitSet(array_count_reg[i][1], j);
						}
						
					}
					//Считаем время задержки
					delay(11);
					if (value_time_delay != 255)
						{
							value_time_delay++;
						}

				}

		  	}
	}
}

void set_push_PCF_to_shiftreg()
{
	//Serial.println("Start push");
	for (i = 0; i < count_reg_PCF; i++)
	{
		byte_PCF_reg = array_count_reg[i][0];

		// Если не нажата ни одна кнопка, то переходим к следующей микросхеме
		if (byte_PCF_reg != 0)
		{
			for (j=0; j < 8; j++)
			{
				/*// Отладка
				Serial.println("i - " + String(i));
				Serial.println("j - " + String(j));
				*/

				bit_PCF_reg = bitRead(byte_PCF_reg, j);
				if (bit_PCF_reg != 0)
				{
					// Проверяем нажатие кнопки и если есть связь со сдвиговым регистром, то инвертируем синганл
					if (bit_PCF_reg == bitRead(array_count_reg[i][2], j))
					{
						value_reg_shift = bitRead(array_shift_reg[i], j);
						bitWrite(array_shift_reg[i], j, !value_reg_shift);
						send_to_serial(i,j,!value_reg_shift);						
					}
					else
					{
						/* code */
					}
					
				}
				
			}
		}
	}

	//устанавливаем отдельные регистры, которые не связаны с PCF
	for (i = count_reg_PCF; i < count_reg_shift; i++)
	{
		for (j=0; j < 8; j++)
		{	
			//bit_PCF_reg = bitRead(array_shift_reg[i], j);

		}
	}

}


void read_serial()
{
	//Serial.println("Start serial");
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
	if (serial_vars.id != "" and serial_num == 4)
	{
		serial_vars.b_id = serial_vars.id.toInt();
		serial_vars.b_topic = serial_vars.topic.toInt();
		serial_vars.b_num = serial_vars.num.toInt();
		serial_vars.b_value = serial_vars.value.toInt();

		Serial.println(serial_vars.topic + "-" + serial_vars.num + "-" + serial_vars.value);
		
		//если номер регистра меньше 50, значит это сдвиговый регистр и ставим его сразу
		if (serial_vars.b_topic < 50)
		{
			//active_reg[0] = serial_vars.b_topic;
			//active_reg[1] = serial_vars.b_num;

			bitWrite(array_shift_reg[serial_vars.b_topic], serial_vars.b_num, serial_vars.b_value);

			//set_reg_shift(serial_vars.b_topic, serial_vars.b_num, serial_vars.b_value, 0);
			//serial_clear();
		}
		// иначе смотрим по обстановке
		else
		{
			/* code */
		}
		
	}
}

// получить значения регистров  0 - № PCF (array_name), 1 - № регистра PCF
byte get_value_regs_PCF(byte num_array_name, byte num_reg)
{
	switch (num_array_name) 
	{
		case 0:
			return expander.readButton(num_reg);
			break;
		case 1:
			return expander_2.readButton(num_reg);
			break;
		default:
			return 0;
	}
}

//Уст бит в сдвиговый регистр 0 - topic, 1 - num, 2 - val, 3 - send отправлять или нет 
void set_reg_shift( bool send)
{
	//Serial.println("Start set_reg_shift");
	//bitWrite(array_shift_reg[topic], num,val);

	//Сдвиг через сдвиговый регистр 74HC595
	digitalWrite(ST_CP, LOW);

	//запись данных в сдвиговые регистры
	for ( i = count_reg_shift - 1; i < 255; i--)
	{
		/*// Отладка
		Serial.println("count_reg_shift - " + String(count_reg_shift));
		Serial.println("i - " + String(i));
		delay(2000);
		*/
		shiftOut(DS, SH_CP, MSBFIRST, array_shift_reg[i]);

	
	}
	
    digitalWrite(ST_CP, HIGH);

	if (send)
		Serial.println("0 - " + String(array_shift_reg[0]) +"1 - " + String(array_shift_reg[1]) );	

}

void serial_clear()
{
	//Serial.println("Start clear");
	serial_vars.id = "";
	serial_vars.topic = "";
	serial_vars.num = "";
	serial_vars.value = "";

	serial_vars.b_id = 0;
	serial_vars.b_topic = 0;
	serial_vars.b_num = 0;
	serial_vars.b_value = 0;

	// Обнуляем регистры нажатых кнопок
	for ( i = 0; i < count_reg_PCF; i++)
	{
		array_count_reg[i][0] = 0;
		array_count_reg[i][1] = 0;
	}

	serial_num = 0;
}
// i1t0n2v1    i1t0n2v0

byte reverse_bit (byte topic, byte num)
{
	// инвертируем бит
	return !bitRead(array_shift_reg[topic], num);
}


// Отправить в Serial
void send_to_serial( byte topic, byte num, int value)
{
	Serial.println("i"+ String(arduino_id)  
	+ 't' + String(topic) 
	+ 'n' + String(num )
	+ 'v' + String(value));
}