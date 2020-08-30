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
#define relays_topic "/home/hall/relay"

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
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);



  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  //delay(500);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  //setSyncProvider(getNtpTime);
  

  	/*client.subscribe("/home/hall/ard1relay4");
	client.subscribe("/home/hall/ard1relay3");
	client.subscribe("/home/hall/ard1relay2");*/
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

/*
bool is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//Страница для ввода логина и пароля
//login page, also called for disconnect

void handleLogin() {
  String msg;
  String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin";
  content += "<p>TIME =" + chtmp + "<br><p>OFF =" + UH + ":" + UM + "<br> <p>noga2 =" + GP2 + "<br> <p>temperature = " + dht.readTemperature() + "<p>humidity = " + dht.readHumidity() + "<br>" ;
  content += "hour:<input type='name' name='UH' placeholder=''><br>";
  content += "min:<input type='name' name='UM' placeholder=''><br>";
  content += "noga2:<input type='name' name='GP2' placeholder=''><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  //if (server.hasArg("UH") && server.hasArg("UM")) {
  if (server.arg("UH") != "" && server.arg("UM") != "") {
    //String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    //server.sendContent(header);
    Serial.println("Log in Successful");
    UH = server.arg("UH").toInt();
    UM = server.arg("UM").toInt();
    Serial.println("ustanovleno " + String(UH, DEC) + ":" + server.arg("UM"));
    temp = 0;
    server.send(200, "text/html", content);
    return;
    //   }
    //    else {
    //    msg = "Vvedite vremya viklucheniya";
    //    Serial.println("Not time");
    //    }
  }
  if (server.arg("GP2") != "") {
    GP2 =   server.arg("GP2").toInt();
    Serial.println("noga 2 v " + String(GP2, DEC));
    server.send(200, "text/html", content);
    return;
  }

  server.send(200, "text/html", content);
}
//Если получен доступ
//root page can be accessed only if authentification is ok

void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  String content = "<html><body><H2>hello, you successfully connected to esp8266!</H2><br>";
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
*/

//##################      Обработка времени, дописать 0      #######################
String convertTime (int digits) {
  String temp = "0";
  if (digits < 10)
    temp += String(digits, DEC) ;
  else
    temp = String(digits, DEC) ;
  return temp;
}


/*void printDigits(int digits) {
  // для цифрового времени, печатает двоеточие и предшествующий 0, если число меньше 10
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


//Вывод цифрового времени
void digitalClockDisplay() {
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}*/


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
    if (client.connect("ESP8266Client")) 
    {
      Serial.println("Mosquitto connect...");
      // Once connected, publish an announcement...
      //client.publish(humidity_topic, "humidity");
      // ... and resubscribe
      /*client.subscribe("/home/hall/ard1relay4");
	   client.subscribe("/home/hall/ard1relay3");
	   client.subscribe("/home/hall/ard1relay2");*/
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

/*
  // Светодиод подлкючен к 5 пину
  // Датчик температуры ds18b20 к 2 пину

  #include <ESP8266WiFi.h>
  #include <PubSubClient.h>
  #include <OneWire.h>
  #include <DallasTemperature.h>

  #define ONE_WIRE_BUS 2
  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);

  const char *ssid = "AIRPORT"; // Имя вайфай точки доступа
  const char *pass = "PASSWORD"; // Пароль от точки доступа

  const char *mqtt_server = "server"; // Имя сервера MQTT
  const int mqtt_port = 11140; // Порт для подключения к серверу MQTT
  const char *mqtt_user = "Login"; // Логи от сервер
  const char *mqtt_pass = "Pass"; // Пароль от сервера

  #define BUFFER_SIZE 100

  bool LedState = false;
  int tm=300;
  float temp=0;

  // Функция получения данных от сервера

  void callback(const MQTT::Publish& pub)
  {
  Serial.print(pub.topic()); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print(pub.payload_string()); // выводим в сериал порт значение полученных данных

  String payload = pub.payload_string();

  if(String(pub.topic()) == "test/led") // проверяем из нужного ли нам топика пришли данные
  {
  int stled = payload.toInt(); // преобразуем полученные данные в тип integer
  digitalWrite(5,stled); // включаем или выключаем светодиод в зависимоти от полученных значений данных
  }
  }

  WiFiClient wclient;
  PubSubClient client(wclient, mqtt_server, mqtt_port);

  void setup() {

  sensors.begin();
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  pinMode(5, OUTPUT);
  }

  void loop() {
  // подключаемся к wi-fi
  if (WiFi.status() != WL_CONNECTED) {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");
  WiFi.begin(ssid, pass);

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  return;
  Serial.println("WiFi connected");
  }

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
  if (!client.connected()) {
  Serial.println("Connecting to MQTT server");
  if (client.connect(MQTT::Connect("arduinoClient2")
  .set_auth(mqtt_user, mqtt_pass))) {
  Serial.println("Connected to MQTT server");
  client.set_callback(callback);
  client.subscribe("test/led"); // подписывааемся по топик с данными для светодиода
  } else {
  Serial.println("Could not connect to MQTT server");
  }
  }

  if (client.connected()){
  client.loop();
  TempSend();
  }

  }
  } // конец основного цикла

  // Функция отправки показаний с термодатчика
  void TempSend(){
  if (tm==0)
  {
  sensors.requestTemperatures(); // от датчика получаем значение температуры
  float temp = sensors.getTempCByIndex(0);
  client.publish("test/temp",String(temp)); // отправляем в топик для термодатчика значение температуры
  Serial.println(temp);
  tm = 300; // пауза меду отправками значений температуры коло 3 секунд
  }
  tm--;
  delay(10);
  }




*/
