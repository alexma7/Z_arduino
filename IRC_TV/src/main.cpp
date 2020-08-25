#include <IRremote.h>

int irPin = 2;

IRrecv irReciver(irPin); // указываем вывод, к которому подключён приёмник
IRsend irsend;

decode_results results;
char id; 

const int buttonPin = 2;     // номер входа, подключенный к кнопке
boolean buttonState = 0; 

void setup() {
  Serial.begin(115200);
  irReciver.enableIRIn(); // запускаем приём
  Serial.println("START");

  pinMode(buttonPin, INPUT); 
}

void loop() {

    buttonState = digitalRead(buttonPin);

    if (buttonState == HIGH) {   
    // включаем светодиод   
    irsend.sendSAMSUNG(0xE0E0E01F, 32);
    delay(80);

  }


    if (Serial.available() > 0) 
    {
        id = Serial.read();
       switch (id) 
		{
			case '1':
				irsend.sendNEC(0xB24D7B84, 32);
                delay(40);
                Serial.println("on");
				break;

            case '2':
				irsend.sendNEC(0xB24DBF40, 32);
                delay(40);
                Serial.println("onhdtr");
				break;

            case '3':
				irsend.sendSAMSUNG(0xE0E0E01F, 32);
                delay(40);
                Serial.println("+s");
				break;

            case '4':
				irsend.sendSAMSUNG(0xB24DBF40 , 32);
                delay(40);
                Serial.println("-s");
				break;

        case '5':
          irsend.sendSAMSUNG(0xB24D1FE0, 32);
          delay(40);
          Serial.println("os");
          break;


        case '6':
          irsend.sendSAMSUNG(0x2A4C28A0088, 48);
          delay(40);
          Serial.println("panOn");
          break;

        }
    }
}
		

  /*if (irReciver.decode(&results)) { 
    Serial.println( results.value, HEX ); 
    irReciver.resume(); */
  

 