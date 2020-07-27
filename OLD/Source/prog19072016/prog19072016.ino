#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

const char* ssid = "alex";
const char* password = "alexma93";

//MQTT
const char *mqtt_server = "192.168.1.37"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
String chtmp;
int UH = 99, UM = 99, GP2 = 3;

#define RELAY_1 2
#define relays_topic "/home/hall/relay"

IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
const int timeZone = 3;     // Central European Time
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

WiFiClient espClient;
PubSubClient client(espClient);
boolean rState1 = true, temp = 0;

ESP8266WebServer server(80);

//Check if header is present and correct

//###########################      SETUP        ########################################################################
void setup(void) {

	delay(200);
	Serial.begin(115200);
	WiFi.begin(ssid, password);

	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	pinMode(RELAY_1, OUTPUT);
	client.setServer(mqtt_server, 1883);
	delay(500);
	client.setCallback(callback);
	delay(500);
	/*Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());*/


	// server.on("/", handleRoot);
	server.on("/login", handleLogin);
	delay(500);
	server.on("/inline", []() {
		server.send(200, "text/plain", "this works without need of authentification");
	});

	server.onNotFound(handleNotFound);
	//here the list of headers to be recorded
	const char * headerkeys[] = { "User-Agent", "Cookie" };
	size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
	//ask server to track these headers
	server.collectHeaders(headerkeys, headerkeyssize);
	delay(500);
	server.begin();
	Serial.println("HTTP server started");

	Serial.print("IP number assigned by DHCP is ");
	Serial.println(WiFi.localIP());
	Serial.println("Starting UDP");
	Udp.begin(localPort);
	delay(500);
	Serial.print("Local port: ");
	Serial.println(Udp.localPort());
	Serial.println("waiting for sync");
	setSyncProvider(getNtpTime);
}

time_t prevDisplay = 0; // when the digital clock was displayed

						//###########################      LOOP        ########################################################################
void loop(void) {
	server.handleClient();
	if (timeStatus() != timeNotSet) {
		if (now() != prevDisplay) { //update the display only if time has changed
			prevDisplay = now();

			// Serial.println(prevDisplay);
			// digitalClockDisplay();
			//  Serial.print("*");
			chtmp = convertTime(hour()) + ":" + convertTime(minute());



			if ((hour() == UH) and (minute() == UM) and (temp == 0)) {
				temp = 1;
				digitalWrite(RELAY_1, temp);
			}

			if (GP2 == 0) {
				digitalWrite(RELAY_1, GP2);
				GP2 = 3;
			}
			if (GP2 == 1) {
				digitalWrite(RELAY_1, GP2);
				GP2 = 3;
			}

		}
	}

	// подключаемся к MQTT серверу
	reconnect_server();

	client.loop();
}//конец loop

 //###########################      OTHER FNC        ########################################################################
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

		return;
		//   }
		//    else {
		//    msg = "Vvedite vremya viklucheniya";
		//    Serial.println("Not time");
		//    }
	}
	if (server.arg("GP2") != "") {
		GP2 = server.arg("GP2").toInt();
		Serial.println("noga 2 v " + String(GP2, DEC));
		return;
	}
	String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin <p>TIME =" + chtmp + "<br><p>OFF =" + UH + ":" + UM + "<br> <p>noga2 =" + GP2 + "<br>";
	//content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
	//content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";

	content += "hour:<input type='name' name='UH' placeholder='hour'><br>";
	content += "min:<input type='name' name='UM' placeholder='min'><br>";
	content += "noga2:<input type='name' name='GP2' placeholder='GP2'><br>";

	content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";


	content += "You also can go <a href='/inline'>here</a></body></html>";
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

void digitalClockDisplay() {
	// digital clock display of the time
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
}

//##################      Обработка времени, дописать 0      #######################
String convertTime(int digits) {
	String temp = "0";
	if (digits < 10)
		temp += String(digits, DEC);
	else
		temp = String(digits, DEC);
	return temp;
}
void printDigits(int digits) {
	// для цифрового времени, печатает двоеточие и предшествующий 0, если число меньше 10
	Serial.print(":");
	if (digits < 10)
		Serial.print('0');
	Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
	while (Udp.parsePacket() > 0); // discard any previously received packets
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
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
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
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}

// Функция получения данных от сервера
void callback(char* topic, byte * payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

	// Switch on the LED if an 1 was received as first character
	if ((char)payload[0] == '1') {
		rState1 = true;
		digitalWrite(RELAY_1, rState1);   // Turn the LED on (Note that LOW is the voltage level
										  // but actually the LED is on; this is because
										  // it is acive low on the ESP-01)
	}
	else {
		rState1 = false;
		digitalWrite(RELAY_1, rState1);  // Turn the LED off by making the voltage HIGH
	}
}

void reconnect_server() {
	if (!client.connected() && WiFi.status() == WL_CONNECTED) {
		if (client.connect("ESP8266Client")) {
			Serial.println("Mosquitto connect...");
			// Once connected, publish an announcement...
			//client.publish(humidity_topic, "humidity");
			// ... and resubscribe
			client.subscribe(relays_topic);
		}
		else {
			Serial.print("failed connect Mosquitto, rc=");
			Serial.print(client.state());
			Serial.println("");
		}
	}
}