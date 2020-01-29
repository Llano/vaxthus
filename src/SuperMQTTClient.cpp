
#include "SuperMQTTClient.hpp"


void SuperMQTTClient::callback(char* topic, byte* payload, unsigned int length) {
 
    std::map<char *, MQTTMessage, cmp_str>::iterator it = this->handlers.find(topic); 
    if (it != this->handlers.end())
    {
        char * ca = new char[length];
        ca[length] = (char)'\0';
        for (u_int i=0;i<length;i++) {
            ca[i] = (char)payload[i];
        }

        Message m = Message(ca);
        m.client = this;
        (it->second)(m);
    }


 
}


void SuperMQTTClient::On(char * topic, MQTTMessage message) {
    char * channel = (char *)malloc((strlen(this->userid) + strlen("/") + strlen(topic)) * sizeof(char) + 1);
    strcpy(channel, this->userid);
    strcat(channel, "/");
    strcat(channel, topic);
    this->handlers.insert(std::pair<char *, MQTTMessage>(channel, message));
    
}

void SuperMQTTClient::Publish(char * topic, char * message) {
    char * channel = (char *)malloc((strlen(this->userid) + strlen("/") + strlen(topic)) * sizeof(char) + 1);
    strcpy(channel, this->userid);
    strcat(channel, "/");
    strcat(channel, topic);
    Serial.print("Trying to send to: ");
    Serial.println(channel);
    Serial.println(this->client.state());
    if (!this->client.publish(channel, message))
    {
        Serial.println("Could not publish message to broker");
    }
    
    
}


bool SuperMQTTClient::Connect() {
    if (client.connect(this->userid))
    {
        Serial.println("Connected to MQTT");
        
        for(const auto& kv : this->handlers) {

            Serial.print("Subscribing to: ");
            Serial.println(kv.first);
            client.subscribe(kv.first);
        }

        return true;
        
    }
    else
    {
        Serial.println("Not connected to MQTT at");
        Serial.print(this->mqtt_address);
        Serial.print(":");
        Serial.println(this->port);

        return false;
    }


}

void SuperMQTTClient::Loop() {

    if (!this->client.connected())
    {
        long now = millis();
        if(now - lastReconnectAttempt > 10000)
        {
            lastReconnectAttempt = now;
            if (this->Connect())
            {
                lastReconnectAttempt = 0;
            }
            
            

        }
    }
    else
    {
        
        this->client.loop();
        
    }
    
    
}
SuperMQTTClient::SuperMQTTClient(const char* url, int port, const char * userid) {

    this->mqtt_address = url;
    this->port = port;
    this->userid = userid;
    this->client.setClient(espClient);
    this->client.setServer(url, port);
    //client.setCallback(callback);
    this->client.setCallback(std::bind(&SuperMQTTClient::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));



    this->handlers = std::map<char *, MQTTMessage, cmp_str>();

    
    
}

