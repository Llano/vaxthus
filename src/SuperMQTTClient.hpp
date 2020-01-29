#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <map>
#include "Message.hpp"


typedef std::function<void(Message message)> MQTTMessage;

class SuperMQTTClient {

    public:
        const char * mqtt_address;
        int port;
        const char * userid;
        SuperMQTTClient(const char* url, int port, const char * userid);
        void On(char * topic, MQTTMessage message);

        void callback(char* topic, byte* payload, unsigned int length);
        bool Connect();
        void Publish(char * topic, char * message);
        void Loop();


        struct cmp_str
        {
            bool operator()(char *first, char  *second)
            {
                return strcmp(first, second) < 0;
            }
            
        };
        PubSubClient client;



    private:
        long lastReconnectAttempt = 0;
        WiFiClient espClient;
        std::map<char *, MQTTMessage, cmp_str> handlers;

};