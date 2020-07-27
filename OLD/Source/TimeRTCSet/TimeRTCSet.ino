#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>
 
void setup()  {
setSyncProvider(RTC.get);
//Устанавливаем время в формате:
//Часы, минуты, секунды, день, месяц, год
setTime(20,57,0,28,11,2016);
//Применяем:
RTC.set(now());
}
void loop()
{
delay(100);
}


