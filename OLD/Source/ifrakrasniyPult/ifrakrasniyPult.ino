#include <IRremote.h>

int RECV_PIN = 2;
int LED = 13;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Запуск приемника
  pinMode(LED, OUTPUT);
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    if (results.value == 33456255) // При получении кодировки 1
    {
      digitalWrite(LED, HIGH); // Включаем светодиод
    }
    if (results.value == 33472575) // При получении кодировки 2
    {
      digitalWrite(LED, LOW); // Выключаем светодиод
    }
    irrecv.resume(); // Получаем следующее значение
  }
}
