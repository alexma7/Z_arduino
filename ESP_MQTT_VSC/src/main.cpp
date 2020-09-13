// G my_proj
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "my_lib.h"
#include "config.h"
//#include <stdlib.h>


// Прототипы функций
void callback(char* topic, byte * payload, unsigned int length);
void sendToMQTT ();
time_t getNtpTime();
void sendNTPpacket(IPAddress & address);
void reconnect_server();


//DHT11
#define DHTPIN 13
#define DHTTYPE DHT11
#define MAINLED 2 
DHT dht(DHTPIN, DHTTYPE);
float humidity, temp_f;


String chtmp;
int UH = 99, UM = 99, GP2 = 3 ;
int tm = 10000;
char ch;

bool savePub = 0;
bool flag_temp = 0;
String str_topic = "";
String str_value = " ";
String strCallback = " ";
String TopicCheck=" ";


#define RELAY_1 2

//Прототипы функций
void callback(char* topic, byte* payload, unsigned int length);


IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
const int timeZone = 3;     // Central European Time
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

WiFiClient espClient;
PubSubClient client(espClient);
void subsPub();
//subsPub();
boolean rState1 = true, temp = 0;
boolean sb = false;

//ESP8266WebServer server(80);



//Check if header is present and correct

//###########################      SETUP        ########################################################################
void setup(void) 
{

  delay(100);
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(100);
    Serial.print(".");
  }

  pinMode(RELAY_1, OUTPUT);
  pinMode(MAINLED, OUTPUT);
  digitalWrite(MAINLED, HIGH);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);



  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  //delay(500);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
}


time_t prevDisplay = 0; // when the digital clock was displayed

//###########################      LOOP        ########################################################################
void loop(void) 
{    
  // подключаемся к MQTT серверу
  reconnect_server();

  //Отправить на mMQTT	 
  sendToMQTT();

  //TempSend();
  client.loop();
}//конец loop


//##########################################################################################################################
//###########################      OTHER FNC        ########################################################################
//##########################################################################################################################
// Получить данные по UART и отправить их на сервер MQTT
void sendToMQTT ()
{
  flag_temp = 0;
  str_topic = "";
  str_value = "";

	while (Serial.available() > 0)
  {
    delay(1);
    ch = Serial.read();

    // Пока идет не значение, мы записываем в топик, как доходим до v - value, так пишем в другую переменную
    if (ch != 'v' and flag_temp == 0)
    {
      str_topic += ch;
    }
    else if (ch == 'v')
    {
      flag_temp = 1;
    }
    else
    {
      str_value += ch;
    }
    
    
  }


  
  //Если адрес ИЛИ значение есть
  if (str_topic != "" || str_value != "" )
  {
    //Serial.println(str_topic);
    //Serial.println(str_value);
		char strAdrBuf[25];
		char strValBuf[10];
		str_topic.toCharArray(strAdrBuf, 25); 
		str_value.toCharArray(strValBuf, 10); 

		if (savePub == 1)
		{
		  client.publish(strAdrBuf, strValBuf, 0);  //Отправляем на сервер MQTT
      Serial.println("On srv save");
      //Serial.println('On srv'+strAdrBuf + strValBuf);
		  savePub=0;
		}
		else
    {
		  client.publish(strAdrBuf, strValBuf);	
      //Serial.println("t|"+str_topic+"|"+str_value);
    }
  }
}

// Функция получения данных от сервера      MQTT  #####################################################
void callback(char* topic, byte * payload, unsigned int length) 
{
  strCallback = "";
  strCallback += String(topic)+"v";
  
  for (unsigned int i = 0; i < length; i++) 
  {
    //Serial.print((char)payload[i]);
	  strCallback+=(char)payload[i];
  }

  Serial.println(strCallback);
}

//##################      Обработка времени, дописать 0      #######################
String convertTime (int digits) 
{
  String temp = "0";
  if (digits < 10)
    temp += String(digits, DEC) ;
  else
    temp = String(digits, DEC) ;
  return temp;
}


/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}


// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress & address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


//Переподключение к серверу MQTT
void reconnect_server() 
{
  if (!client.connected() && WiFi.status() == WL_CONNECTED) 
  {
    if (client.connect(mqtt_name, mqtt_login, mqtt_password)) 
    {
      Serial.println("Mosquitto connect...");
      client.subscribe("#");
    } 
    else 
    {
      Serial.print("failed connect Mosquitto, rc=");
      Serial.print(client.state());
      Serial.println("");
    }
  }
}


// Функция отправки показаний с термодатчика
void TempSend() 
{
  if (tm == 0)
  {
    String h = String(int(dht.readHumidity()), DEC); //, t = dht.readTemperature();
    char test[20];
    client.publish("/hall/tem", dtostrf(dht.readTemperature(), 2, 2, test));  // отправляем в топик для термодатчика значение температуры
    client.publish("/hall/hum", dtostrf(dht.readHumidity(), 2, 2, test));
    //Serial.println(temp);
    tm = 10000; // пауза меду отправками значений температуры коло 3 секунд
  }
  tm--;
  delay(10);
}


//Подписаться на паблики
void subsPub()
{

}	