#include "robotESPcom.h"

//#define DEBUG

#define dirA D3
#define dirB D4
#define enA D1
#define enB D2

WiFiServer server(1234);
WiFiClient client;

char commandBuffer[100];
char returnBuffer[100];
boolean returnReady = false;


void setupWiFi() {
	WiFi.mode(WIFI_AP);
	WiFi.softAP("RControl");
}

void setup() {

	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(dirA, OUTPUT);
	pinMode(dirB, OUTPUT);

	setupWiFi();
	server.begin();
#ifdef DEBUG
	delay(2000);
#endif
	Serial.begin(115200);

#ifdef DEBUG

	delay(250);
	Serial.println();
	Serial.println("Hello");
	Serial.flush();
	delay(750);
	Serial.print("IP address");
	Serial.println(WiFi.localIP());
	delay(500);
#endif
}

void loop() {

	static boolean packetActive = false;
	static uint8_t index = 0;
	static boolean readingReturn = false;
	static uint8_t rindex = 0;

	if (!client.connected()) {
		// try to connect to a new client
		client = server.available();
	} else {
		// read data from the connected client
		if (client.available() > 0) {
			char c = client.read();

			if (packetActive) {
				commandBuffer[index] = c;
				commandBuffer[++index] = 0;
				if (c == EOP) {
					comHandler();
					packetActive = false;
				}
			}

			else if (c == SOP) {
				index = 0;
				commandBuffer[index] = c;
				commandBuffer[++index] = 0;
				packetActive = true;
			}

			if(returnReady){
				client.print(returnBuffer);
				returnReady = false;
			}
		}
	}


	if(Serial.available() && !returnReady){
		char s = Serial.read();

		if(s == SOP){
			readingReturn = true;
			rindex = 0;
		}
		if (readingReturn){
			returnBuffer[rindex] = s;
			returnBuffer[++rindex] = 0;

			if(s == EOP){
				returnReady = true;
			}
		}
	}
}

void comHandler() {

	if (commandBuffer[0] == SOP) {
		switch (commandBuffer[1]) {

		case 'M':
			motorCommand();
			break;

		case 'S':
			//servo command
			Serial.print(commandBuffer);
			break;

		case 'R':
			// request for data
			Serial.print(commandBuffer);
			break;

		default:
			return;
		}
	}
}

void motorCommand() {
	if (commandBuffer[2] == 'R') {
		if (commandBuffer[4] == '1') {
			digitalWrite(dirA, HIGH);
			digitalWrite(enA, HIGH);
		} else if (commandBuffer[4] == '-' && commandBuffer[5] == '1') {
			digitalWrite(dirA, LOW);
			digitalWrite(enA, HIGH);
		} else {
			digitalWrite(enA, LOW);
		}
	}

	else if (commandBuffer[2] == 'L') {
		if (commandBuffer[4] == '1') {
			digitalWrite(dirB, LOW);
			digitalWrite(enB, HIGH);
		} else if (commandBuffer[4] == '-' && commandBuffer[5] == '1') {
			digitalWrite(dirB, HIGH);
			digitalWrite(enB, HIGH);
		} else {
			digitalWrite(enB, LOW);
		}
	}
}
