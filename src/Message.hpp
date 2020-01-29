#include <PubSubClient.h>

class SuperMQTTClient;
class Message {
    public:
        char * message;
        unsigned int length;
        SuperMQTTClient * client;

        Message(char * message);
};