/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>

const char* ssid = "alex";
const char* password = "alexma93";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);  //Устанавливаем второй порт на вывод
  digitalWrite(2, 0);  //Записывает 0
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);  //Подключение к вафле
  
  while (WiFi.status() != WL_CONNECTED) {  //Ждем подключения, печатаются точки, если подключились идем дальше
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();  //Старт сервера
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());  //Печать айпи в консоли
}

void loop() {           //Зацикленная функция
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = "|"+client.readStringUntil('\r')+"|";  // записывается строка типа "GET /gpio/1 HTTP/1.1"
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/gpio0=0") != -1)
    val = 0;
  else if (req.indexOf("/gpio0=1") != -1 or req.indexOf("") != 1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(2, val);  //Установка пина по запросу
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val)?"high":"low";
  s += "</html>\n";
  s += "<body><p><a href='gpio0=1'><button class=btn>00 - ON</button></a>";
  s += "<a href='gpio0=0'><button class=btn>00 - OFF</button></a>";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

