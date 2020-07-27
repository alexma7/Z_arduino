int IN1 = 2; //input1 подключен к выводу 8 

 
void setup()
{
  pinMode (IN1, OUTPUT);
  digitalWrite (IN1, HIGH); 
}
void loop()
{
digitalWrite (IN1, HIGH); 
delay(1000);
digitalWrite (IN1, LOW); 
delay(3000);
  


  }
