#include <Arduino.h>
//#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ThingsBoard.h>

#include "Utils/LedBink.hpp"
#include "SuperMQTTClient.hpp"
#include <EEPROM.h>


unsigned long ledtime = 0;
bool blinkStatus = true;
SuperMQTTClient * mqttclient;
WiFiManager wifiManager;
WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

struct ThingsboardAuth {
  char * url;
  char * apikey;
};


int sendDataDelay = 5000;

RPC_Response getValue(const RPC_Data &data){
  float getval = data;
  Serial.print("Received the getInterval method");
  Serial.println(getval);
  return RPC_Response(NULL, sendDataDelay);
}

RPC_Response setValue(const RPC_Data &data){
  Serial.println("Received the setDelay method");
  Serial.print("SetInterval value: ");
  Serial.println(sendDataDelay);
  sendDataDelay = data;

  return RPC_Response(NULL, sendDataDelay);
}

boolean delay_without_delaying(unsigned long &since, unsigned long time) {
  // return false if we're still "delaying", true if time ms has passed.
  // this should look a lot like "blink without delay"
  unsigned long currentmillis = millis();
  if (currentmillis - since >= time) {
    since = currentmillis;
    return true;
  }
  return false;
}


void configModeCallback (WiFiManager *myWiFiManager) {

	//digitalWrite(LED_BUILTIN, HIGH);
	digitalWrite(LED_BUILTIN, LOW);

	
	
}

void LoadSettings(char settings[2][21]) {
	EEPROM.begin(512);
	int index = 0;
	for (size_t i = 0; i < 2; i++)
	{
		EEPROM.get(index, settings[i]);

		index = index + sizeof(settings[i]);

	}
	EEPROM.end();



}
/** Store WLAN credentials to EEPROM */
void SaveSettings(char settings[][21], int size) {

	
	EEPROM.begin(512);
	int index = 0;
	for (size_t i = 0; i < size; i++)
	{
		EEPROM.put(index, settings[i]);
		index = index + sizeof(settings[i]);
	}

	EEPROM.commit();
	EEPROM.end();
}

bool shouldSaveConfig = false;
void saveConfigCallback () {
  
  shouldSaveConfig = true;
}


RPC_Callback callbacks[] = {
  { "setValue", setValue },
  {"getValue", getValue}};


void setup() {
	Serial.begin(9600);
	
	pinMode(0, INPUT_PULLUP);

	char settings[2][21];
	LoadSettings(settings);
	Serial.println(settings[0]);
	Serial.println(settings[1]);
	uint32_t chipid = ESP.getChipId();
	char cstr[30];
	
	//itoa(chipid, cstr, 20);
	snprintf(cstr, 20, "AP-%d", chipid);
	char chipid_str[20];
	sprintf(chipid_str, "%d", ESP.getChipId());
	char * chip_string = (char *)malloc(strlen(chipid_str) * sizeof(char) + 1);

	strcpy(chip_string, chipid_str);
	wifiManager.setAPCallback(configModeCallback);
	wifiManager.setSaveConfigCallback(saveConfigCallback);
	WiFiManagerParameter thingsboard("thingsboard", "Thingsboard address", settings[0], 40);
	WiFiManagerParameter apikey("apikey", "Thingsboard apikey", settings[1], 40);

	// WiFiManagerParameter mqtt_address("mqttaddress", "MQTT address", "192.168.0.12", 40);
	// WiFiManagerParameter mqtt_port("mqttport", "MQTT port", "1883", 10);
	wifiManager.addParameter(&thingsboard);
	wifiManager.addParameter(&apikey);

	//wifiManager.addParameter(&mqtt_address);
	//wifiManager.addParameter(&mqtt_port);

	wifiManager.setAPStaticIPConfig(IPAddress(8,8,8,8), IPAddress(8,8,8,8), IPAddress(255,255,255,0));
	if(wifiManager.autoConnect(cstr)) {
		//const char* chose_mqtt_address = mqtt_address.getValue();
		//const char* chosen_mqtt_port = mqtt_port.getValue();

		// mqttclient = new SuperMQTTClient(chose_mqtt_address, atoi(chosen_mqtt_port), chip_string);



		// mqttclient->On("test", [](Message m) {
		// 	Serial.println(m.message);
		// });


		// mqttclient.On("reset", [](Message m) {
		// 	Serial.println("resetting..");
		// 	WiFi.disconnect();
		// 	ESP.restart();
		// });

		
		// mqttclient->On("battery", [](Message m) {

			

		// 	//m.client->Publish("battery", batteryLevel);
		// });


		
		// mqttclient->Connect();


			 

	}

	if (shouldSaveConfig)
	{
		char arr[2][21];
		strcpy(arr[0], thingsboard.getValue());
		strcpy(arr[1], apikey.getValue());
		Serial.println(arr[0]);
		Serial.println(arr[1]);


		SaveSettings(arr, 2);
	}

	Serial.println(strlen(settings[1]));
	if(tb.connect(settings[0], settings[1])) {
		Serial.println("Connected to ThingsBoard");
		if (tb.RPC_Subscribe(callbacks, 2)) {
			Serial.println("Connected to RPC");
			return;
		}
	}else
	{
		
		Serial.println("Couldn't connect to thingsboard");
	}
	
	

	
	
	
	
}
long lastMillis = 0;
char batteryLevel[10];


LedBlink b = LedBlink(16, 1000);

void loop() {

	// mqttclient->Loop();
	tb.loop();
	
	if (digitalRead(0) == LOW)
	{
		WiFi.disconnect(true);
		delay(2000);
		ESP.reset();
	}

	
	// sprintf(batteryLevel, "%d", ESP.getVcc());
	long currentMillis = millis();
	if(currentMillis - lastMillis > sendDataDelay) {
		// digitalWrite(LED_BUILTIN, LOW);

	  	// Serial.println(batteryLevel);
	 	lastMillis = millis();
		Serial.println(analogRead(A0));
	 	// mqttclient->Publish("battery", batteryLevel);
		tb.sendTelemetryFloat("temperature", analogRead(A0));
	}

	// b.Update();

	// delay(10);

	//Serial.println(mqttclient->client.connected());


	// Serial.println("NO");
    // if(delay_without_delaying(ledtime, 5000) && blinkStatus)
	// {
	// 	Serial.println("GO");
	//  	ledstate = !ledstate;

	// 	digitalWrite(LED_BUILTIN, ledstate);
	// }
}

