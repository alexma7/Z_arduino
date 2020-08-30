#include "my_lib.h"
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define speacRelay      8

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

  return clockTemp = String(hour())+printDigits(minute()) + printDigits(second());
}

String printDigits(int num) 
{
	String a =":";
	// utility function for digital clock display: prints preceding colon and leading 0
	//Serial.print(":");
	if (num < 10)
	a+="0";
	return a+=String(num);
	// Serial.print(num);
}


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