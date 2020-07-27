int sensePin = 0;
int ledPin = 9;

void setup() {
 analogReference (DEFAULT);

pinMode (ledPin, OUTPUT);
//Serial.begin (9600); 
}

void loop() {
 //Serial.println(analogRead(sensePin));
 //delay(500);
 int value = analogRead(sensePin);
 value = constrain (value,);

}
