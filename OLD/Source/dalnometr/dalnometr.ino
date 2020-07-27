int echoPin = 9; 
int trigPin = 8; 
int red = 7;
int green = 6;
int blue = 5;

void setup() { 
  Serial.begin (9600); 
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(red, OUTPUT); 
  pinMode(green, OUTPUT); 
  pinMode(blue, OUTPUT); 
} 

void loop() { 
  int duration, cm;
  
  digitalWrite(red, HIGH); 
  digitalWrite(green, HIGH); 
  digitalWrite(blue, HIGH); 
  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH); 
  cm = duration / 58;
  if (cm>1)
{
  if (cm<100)
  {
     digitalWrite(red, LOW); 
     digitalWrite(green, HIGH); 
     digitalWrite(blue, HIGH); 
    }
     if (cm<50)
  {
     digitalWrite(red, HIGH); 
     digitalWrite(green, LOW); 
     digitalWrite(blue, HIGH); 
    }
     if (cm<30)
  {
     digitalWrite(red, HIGH); 
     digitalWrite(green, HIGH); 
     digitalWrite(blue, LOW); 
    }
  Serial.print(cm); 
  Serial.println(" cm"); 
  delay(100);
}
}
