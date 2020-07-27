int GLED= 13; // мокрая почва D13
int RLED= 12; // сухая почва D12
int SENSE= 0; // аналоговый сигнал датчика влажности почвы A0
int value= 0;
void setup() {
   Serial.begin(9600);
   pinMode(GLED, OUTPUT);
   pinMode(RLED, OUTPUT);
   Serial.println("SOIL MOISTURE SENSOR");
   Serial.println("-----------------------------");
}
void loop() {
   value= analogRead(SENSE);
   value= value/10;
   Serial.println(value);
   if(value<50)
   {
      digitalWrite(GLED, HIGH);
   }
   else
   {
      digitalWrite(RLED,HIGH);
   }
   delay(1000);
   digitalWrite(GLED,LOW);
   digitalWrite(RLED, LOW);
}
