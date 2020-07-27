//Тестировалось на Arduino IDE 1.0.5
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);   /* Задаем адрес и размерность дисплея. 
При использовании LCD I2C модуля с дисплеем 20х04 ничего в коде изменять не требуется, cледует только задать правильную размерность */

void setup()
{
  lcd.init();                            // Инициализация lcd             
  lcd.backlight();                       // Включаем подсветку
  // Курсор находится в начале 1 строки
  lcd.print("Hello, world!");            // Выводим текст
  lcd.setCursor(0, 1);                   // Устанавливаем курсор в начало 2 строки
  lcd.print("zelectro.cc");              // Выводим текст
}

void loop()
{
}
