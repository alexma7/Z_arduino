#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей
const int sensTherm = A0; 
const int voltMetr = A1; 


#define btnOff      4

int abcd = 5;
int zoomer = 3;

float R1 = 100700.0;
float R2 = 10045.0;
float a = 0.0;
float b = 0.0;
float koefTochnosty = 1.01;
int value = 0;
int counter = 0;
byte offTimer =60;
byte onZoomer = 0;
float sumVolt = 0;
//const float typVbg = 1.179;
void setup()
{
	//Установка портов на
	//Выход
	pinMode(abcd, OUTPUT);
	pinMode(zoomer, OUTPUT); 	

	//Вход
	pinMode(btnOff, INPUT); 
	
	//Установка портов
	
	
	Serial.begin(115200);
	lcd.init();                     
	lcd.backlight();// Включаем подсветку дисплея
	//lcd.print("Nastiya LOH");

	//analogReference(INTERNAL);
}
void loop()
{
	
	while (counter<20)
	{
		
		value = analogRead(voltMetr);
		a = value*5/1024.0;
		b = a/(R2/(R1+R2))*koefTochnosty;
		sumVolt+=b;
		if (onZoomer = 1){
			digitalWrite(zoomer, LOW);
			delay(100);
			digitalWrite(zoomer, LOW);
			offTimer-=2;
		} else{
			digitalWrite(abcd, HIGH);
			digitalWrite(zoomer, HIGH);
			delay(100);
		}
		counter++;
	
	}
	
	
	b=sumVolt/20;
	
		Serial.println("----------------");
	Serial.println(value);
	Serial.println(a);
	Serial.println(b);
	lcd.setCursor(0, 0);
	if (b<=11.2){
		lcd.print("RAZRYAD! OFF-"+offTimer);
		onZoomer = 1;
	} else	if (b<=12.5){
		offTimer=60;
		onZoomer = 0;
		lcd.print("Nizkij zaryad!");
	} else if(b>12.5){
		offTimer=60;
		onZoomer = 0;
	}
	lcd.setCursor(8, 1);
	lcd.print(b);
	lcd.print("V");
	//String(float(Thermister(analogRead(sensTherm))),1)
  // Устанавливаем курсор на вторую строку и нулевой символ.
  lcd.setCursor(0, 1);
  // Выводим на экран количество секунд с момента запуска ардуины
  //lcd.print(millis()/1000);
  
  lcd.print(Thermister(analogRead(sensTherm)));
  
  //если время кончилось, то выелючить устройство
  if (offTimer=0){
	 digitalWrite(abcd, LOW);
  }
  //Обнуление
  counter=0;
  sumVolt=0;
}
//Термистор
double Thermister(int RawADC) {
  double Temp;
  Temp = log(((10240000/RawADC) - 9870));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;   // Kelvin to Celcius
  //Temp = (Temp * 9.0)/ 5.0 + 32.0;    // 1 способ Convert Celcius to Fahrenheit 
  //Temp = (Temp * 1.8) + 32.0;   // 2 способ Convert Celcius to Fahrenheit
  return Temp;
}
