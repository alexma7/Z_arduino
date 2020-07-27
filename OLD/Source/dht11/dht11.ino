
#include <DHT.h>
#define DHTPIN 13  //dht11 data in
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float humidity, temp_f; // Values read from sensor

// create a function called "sendData" and repeat it every x minutes, refer to line 162
void sendData(){

    float h = dht.readHumidity();
    delay(200);
    float t = dht.readTemperature();

    Serial.print("temperature C");
    Serial.println( t );
    delay(500);
    Serial.print("humidity %");
    Serial.println( h );
}


void setup(){

  Serial.begin(9600); 
  dht.begin();
}


// everything is setup, now the system starts until power off
void loop(){


  sendData();
  delay(1000);
}
  
