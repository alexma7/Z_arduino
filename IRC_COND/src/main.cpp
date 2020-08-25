#include <IRremote.h> //библиотека для работы с инфракрасными передатчиком и приемником
#include <Arduino.h>
#include <avr/pgmspace.h>

// Объявление прототипов
// Чтение и установка данных из Serial
void read_serial();

// очищаем переменные Serial
void serial_clear();

void generate_irc();


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
String arduino_id = "3";


IRsend irsend;

int irPin = 2;

decode_results results;
char id; 

int khz = 38; // несущая частота 38 кГц для протокола NEC

int PevData;

// Охлаждение
const PROGMEM unsigned int cool24[] = {4350,4350,550,1650,500,550,550,1650,500,1600,600,500,550,550,550,1600,550,550,550,500,600,1600,550,500,600,500,550,1600,550,1600,550,600,450,1650,600,1600,500,550,550,1650,500,1650,550,1600,500,1650,550,1650,500,1650,550,550,500,1650,500,600,500,600,550,500,500,600,500,600,500,600,500,550,500,1650,500,600,500,550,550,550,500,600,500,550,550,550,600,1600,500,600,450,1700,500,1650,500,1650,500,1700,450,1650,550,1650,550};

const PROGMEM unsigned int cool25[] = {4350,4350,550,1650,500,550,550,1650,500,1650,550,550,500,550,550,1650,500,550,550,550,550,1600,550,550,550,550,500,1650,550,1600,550,550,550,1600,550,1600,550,600,500,1650,500,1650,550,1600,550,1600,550,1650,500,1650,550,500,550,1650,550,550,500,550,550,550,550,550,500,550,550,550,550,1650,500,1650,500,600,500,550,550,550,500,550,550,550,550,550,550,500,550,600,500,1600,550,1650,500,1650,550,1600,550,1600,550,1650,550};

const PROGMEM unsigned int cool26[] ={ 4300,4400,500,1650,500,600,500,1650,500,1650,500,600,500,600,550,1600,500,600,500,550,550,1650,550,500,550,550,550,1600,500,1700,500,550,500,1700,500,1600,550,600,450,1700,550,1600,550,1600,550,1650,450,1650,600,1600,500,600,550,1600,500,600,500,550,550,550,550,550,500,550,500,600,550,1600,550,1650,500,550,500,1700,500,600,450,600,550,550,500,600,500,550,500,600,550,1600,550,550,500,1650,550,1600,550,1600,600,1600,500};

// Нагрев
const PROGMEM unsigned int heat27[] = {4400,4350,500,1650,550,550,500,1650,550,1600,550,550,550,550,500,1650,550,550,550,500,550,1650,500,550,550,550,550,1600,550,1600,550,600,500,1650,500,1650,550,500,550,1650,550,1600,550,1600,550,1650,500,1650,550,1600,550,550,550,1600,550,550,550,550,500,550,550,550,550,550,500,550,550,1650,500,550,550,550,550,1600,550,1650,500,1650,550,500,550,550,550,550,550,1600,550,1600,550,550,550,550,550,550,500,1650,550,1600,550};

const PROGMEM unsigned int heat29[] = {4400,4350,500,1650,500,600,500,1650,550,1650,500,550,500,600,500,1650,500,600,500,600,500,1650,500,550,550,550,500,1650,550,1650,500,600,450,1700,500,1650,500,600,500,1650,500,1650,500,1650,550,1650,500,1650,550,1600,550,550,500,1700,450,600,500,600,500,600,500,550,500,600,500,600,500,1600,550,550,500,1700,500,600,550,1600,500,1650,500,550,550,550,550,550,500,1700,450,600,500,1700,450,600,500,600,500,1650,500,1650,550};

// Управление
const PROGMEM unsigned int off[] = {4400,4300,550,1650,500,550,500,1700,500,1650,500,550,550,550,500,1700,500,550,550,550,550,1600,500,600,500,550,550,1650,500,1650,550,550,550,1600,550,550,500,1650,500,1650,500,1700,500,1650,500,600,500,1650,500,1650,550,1600,550,550,500,600,500,600,500,550,500,1700,500,550,500,600,500,1650,500,1650,500,1700,500,550,500,600,500,600,500,550,550,550,550,550,500,550,550,550,500,1650,600,1600,500,1650,500,1650,500,1650,550};

const PROGMEM unsigned int off2_5h[] = {4350,4350,550,1600,600,500,550,1650,550,1600,550,500,600,500,550,1600,600,500,600,500,550,1600,550,550,550,550,550,1600,550,1600,550,550,550,1600,600,1550,600,500,550,1600,600,500,550,1600,600,500,550,550,550,1600,600,500,550,1600,550,550,550,1600,600,500,550,1600,600,1600,550,500,600,500,550,1600,600,500,550,550,550,500,600,500,550,550,550,550,550,500,550,1650,550,1600,550,1600,550,1600,600,1600,550,1600,550,1600,600};

const PROGMEM unsigned int off1_5h[] = {4400,4300,600,1600,550,550,550,1600,550,1600,550,550,550,550,550,1600,550,550,550,500,550,1600,600,500,550,550,550,1600,550,1600,600,500,550,1600,600,1600,550,500,600,1600,550,500,600,500,550,1600,600,1600,550,1600,550,550,550,1600,550,550,550,1600,550,1600,550,550,550,550,550,500,600,500,550,1600,600,500,550,550,550,500,600,500,550,550,550,550,500,550,550,1650,550,1600,550,1600,550,1600,600,1600,550,1600,550,1600,550};

const PROGMEM unsigned int led[] = {4350,4400,500,1650,500,600,500,1650,500,1700,500,550,500,1650,550,550,500,1700,500,550,500,1700,500,600,450,600,500,1700,450,600,500,1700,450,600,550,1650,450,1700,500,1650,500,1650,500,600,500,1650,500,600,500,1650,550,550,500,600,500,550,500,600,500,1650,500,600,500,1650,500,600,550,1600,500,600,500,1650,500,600,500,600,500,1650,500,600,500,1650,550,550,500,1650,500,600,500,1650,500,1700,450,600,500,1700,450,600,550};

// таблица ссылок кондиционера
const  unsigned int* const array_cond[] PROGMEM = {off, cool24, cool25, cool26, heat27, heat29, off2_5h, off1_5h, led};

// Команды для ресивера
/*
Encoding  : PANASONIC
Code      : 0x2A4C:28A0088 (48 bits)
*/
unsigned int res_on_off[] ={3350,1650,450,350,450,400,450,1250,400,450,400,1250,450,350,450,1250,450,400,400,400,450,1250,450,400,400,400,450,1250,400,1250,450,400,450,400,400,450,400,400,450,400,450,350,450,450,400,400,450,1250,400,400,450,1200,450,450,400,400,450,400,450,1200,450,400,450,1200,450,400,450,400,450,400,400,400,450,400,450,400,400,450,400,400,450,400,450,1250,400,400,450,400,450,350,450,1250,450,400,400,450,400,400,450};

/*
Encoding  : PANASONIC
Code      : 0x2A4C:284B432 (48 bits)
*/
const PROGMEM unsigned int res_tv[] ={3350,1650,400,400,450,400,450,1200,450,400,450,1250,400,400,450,1250,450,350,450,400,450,1250,400,400,450,400,450,1200,450,1250,400,450,400,400,450,400,450,400,450,400,400,400,450,400,450,400,400,1250,450,400,400,1250,450,400,400,450,450,350,450,400,450,1250,400,400,450,400,400,1250,450,400,450,1250,400,1250,400,450,400,1250,400,450,400,450,400,400,450,400,450,1200,450,1250,400,450,400,400,450,1250,400,400,450};

/*
Encoding  : PANASONIC
Code      : 0x2A4C:28AB43C (48 bits)
*/
const PROGMEM unsigned int res_game[] ={3400,1600,450,400,450,400,450,1200,450,400,450,1200,450,400,450,1250,400,400,450,400,450,1200,450,400,450,400,400,1250,450,1250,400,400,450,400,450,400,450,400,400,400,450,400,450,400,450,400,400,1250,450,400,450,1200,450,400,450,400,400,400,450,1250,450,350,450,1250,450,400,400,1250,450,400,450,1200,450,1200,450,400,450,1250,400,400,450,400,450,400,450,400,400,1250,450,1200,450,1250,450,1200,450,400,450,400,400};


/*Encoding  : PANASONIC
Code      : 0x2A4C:28F74F9 (48 bits)
*/
const PROGMEM unsigned int res_bluetooth[] ={3350,1650,400,400,450,400,450,1200,450,400,450,1250,400,400,450,1250,450,350,450,400,450,1250,400,400,450,400,450,1200,450,1250,450,400,400,400,450,400,450,400,450,400,400,400,450,400,450,400,450,1200,450,400,450,1200,450,400,450,400,450,400,400,1250,450,1200,450,1250,450,1200,450,400,450,1200,450,1250,400,1250,450,400,450,1200,450,400,450,400,400,1250,450,1200,450,1250,450,1200,450,1250,400,400,450,400,450,1200,450};



unsigned int ir_buf[99];
//int i;

// ПУЛЬТ ТЕЛЕВИЗОРА
unsigned long tv_command[] = 
{
    0xE0E040BF, // Вкл/выкл
    0xE0E020DF, // 1
    0xE0E0A05F, // 2
    0xE0E0609F, // 3
    0xE0E010EF, // 4
    0xE0E0906F, // 5
    0xE0E050AF, // 6
    0xE0E030CF, // 7
    0xE0E0B04F, // 8
    0xE0E0708F, // 9
    0xE0E08877, // 0
    0xE0E0E01F, // громкость +
    0xE0E0D02F, // громкость -
    0xE0E048B7, // вперед 
    0xE0E008F7, // назад
    0xE0E006F9, // вверх
    0xE0E08679, // вниз
    0xE0E0A659, // лево
    0xE0E046B9, // право
    0xE0E016E9, // центр
    0xE0E0F00F, // выключить звук
    0xE0E0807F, // source
    0xE0E01AE5, // назад
    0xE0E0B44B, // выход
    0xE0E058A7, // настройки
    0xE0E0F20D, // гайд
    0xE0E0D629, // ch list
    0xE0E09E61, // дом
    0xE0E0FC03, // мануал
    0xE0E0F807, // инфо
    0xE0E036C9, // A
    0xE0E028D7, // B
    0xE0E0A857, // C
    0xE0E06897, // D
    0xE0E07C83, // pic size
    0xE0E014EB, // picture
    0xE0E0A45B, // ad/subt
    0xE0E0629D, // остановить
    0xE0E0A25D, // перемотка назад
    0xE0E0E21D, // запуск видео
    0xE0E052AD, // пауза
    0xE0E012ED, // перемотка вперед
};

void setup() 
{
  Serial.begin(115200);
  Serial.println("START");
}
void loop()
{  
    read_serial();
    if (serial_vars.id != "")
    {
        generate_irc();
    }
    if (serial_vars.id != "")
    {
        serial_clear(); 
    }
    
}



// ФУНКЦИИ
void read_serial()
{
	//Serial.println("Start serial");
	while (Serial.available() > 0)
	{
		//delay(1);
		// забираем первый символ
		serial_char = Serial.read();
  
        Serial.println(serial_char);
	 	switch (serial_char) 
		{
			case 'i':
                Serial.println("+");
				serial_num = 1;
				continue;

			case 't':
				// если id сообщения равен устройству, то записываем следующее значение
				if (serial_vars.id == arduino_id)
				{
                    Serial.println("*");
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
                Serial.println("$");
				serial_num = 3;
				continue;

			case 'v':
                Serial.println("^");
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

        Serial.print("SW-");
        Serial.println(serial_num, DEC);

		switch (serial_num) 
		{
			case 1:
				serial_vars.id += serial_char;
                Serial.println(serial_vars.id);
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
	}
}

void serial_clear()
{
	Serial.println("Start clear");
	serial_vars.id = "";
	serial_vars.topic = "";
	serial_vars.num = "";
	serial_vars.value = "";

	serial_vars.b_id = 0;
	serial_vars.b_topic = 0;
	serial_vars.b_num = 0;
	serial_vars.b_value = 0;

	serial_num = 250;
}

void generate_irc()
{
    switch (serial_vars.b_topic)
        {
            case 1: //для кондеев
            {
                
                /*switch (serial_vars.b_value)
                {
                    case 1: // обогрев
                    {
                        	irsend.sendRaw(heat27, sizeof(heat27) / sizeof(heat27[0]), khz);
                            break;
                    }

                   case 2: // обогрев
                    {
                        	irsend.sendRaw(heat29, sizeof(heat29) / sizeof(heat29[0]), khz);
                            break;
                    }

                     case 3: // Охлаждение
                    {
                        //i = 99;
                        //memcpy_P ( ir_buf, &cool24[i], i);
                        for (byte i = 0; i <= 99; i++) {
                            ir_buf[i] = pgm_read_word(&cool24[i]);
                        }
  

                        irsend.sendRaw(ir_buf, sizeof(ir_buf) / sizeof(ir_buf[0]), khz);
                        
                        Serial.print("cond 3-" + ir_buf[0]);
                        Serial.print("-" + ir_buf[1]);
                        Serial.print("-" + ir_buf[2]);
                        Serial.println("-" + ir_buf[3]);
                        break;
                    }

                    case 4: // Охлаждение
                    {
                        	irsend.sendRaw(cool25, sizeof(cool25) / sizeof(cool25[0]), khz);
                            break;
                    }

                    
                    case 5: // Охлаждение
                    {
                        	irsend.sendRaw(cool26, sizeof(cool26) / sizeof(cool26[0]), khz);
                            break;
                    }           

                    case 6: // Управление
                    {
                        	irsend.sendRaw(off, sizeof(off) / sizeof(off[0]), khz);
                            break;
                    }           

                    case 7: // Управление
                    {
                        	irsend.sendRaw(off1_5h, sizeof(off1_5h) / sizeof(off1_5h[0]), khz);
                            break;
                    }           

                    case 8: // Управление
                    {
                        	irsend.sendRaw(off2_5h, sizeof(off2_5h) / sizeof(off2_5h[0]), khz);
                            break;
                    }           

                    case 9: 
                    {
                        	irsend.sendRaw(led, sizeof(led) / sizeof(led[0]), khz);
                            break;
                    }
                }*/

                //i = 99;

                //memcpy_P ( ir_buf, &cool24[i], i);
                Serial.println("condStart");
                for (byte i = 0; i < 99; i++) {
                    ir_buf[i] = pgm_read_word(&array_cond[1][i]);
                }

                Serial.println("condEnd" +  ir_buf[98]);


                
                
                Serial.print("cond 3-" + ir_buf[0]);
                Serial.print("-" + ir_buf[1]);
                Serial.print("-" + ir_buf[2]);
                Serial.println("-" + ir_buf[3]);
                irsend.sendRaw(ir_buf, sizeof(ir_buf) / sizeof(ir_buf[0]), khz);
                break;
                //Serial.print(tv_command[serial_vars.b_value]);
                //Serial.println("cond");
                delay(1000);
                //Serial.println("cond");
                

				break;
            }

            case 2: //для ТV самсунг
            {
                irsend.sendSAMSUNG(tv_command[serial_vars.b_value], 32);
                //Serial.print(tv_command[serial_vars.b_value]);
                //Serial.println("TV");
                delay(80);
            };

           /* case 3: //для ресивера
            {
                switch (serial_vars.b_value)
                {
                    case 0: // включить/выкл
                    {
                        	irsend.sendRaw(heat27, sizeof(heat27) / sizeof(heat27[0]), khz);
                            break;
                    }

                    case 1: // TV
                    {
                        	irsend.sendRaw(res_tv, sizeof(res_tv) / sizeof(res_tv[0]), khz);
                            break;
                    }

                    case 2: // game
                    {
                        	irsend.sendRaw(res_game, sizeof(res_game) / sizeof(res_game[0]), khz);
                            break;
                    }

                    case 3: // bluetooth
                    {
                        	irsend.sendRaw(res_bluetooth, sizeof(res_bluetooth) / sizeof(res_bluetooth[0]), khz);
                            break;
                    }
                }
            }*/

        }
}