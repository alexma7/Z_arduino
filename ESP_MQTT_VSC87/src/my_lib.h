#ifndef __MY_LIB
#define __MY_LIB
#include <Arduino.h>

double Thermister(int RawADC);
String digitalClockDisplay();
String printDigits(int num);
void setTimeFromFormatString(String time);
void checkTime ();
#endif
