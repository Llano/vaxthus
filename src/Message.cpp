#include "Message.hpp"

#include <stdlib.h>
#include <string.h>
Message::Message(char * message) {
    //this->message = (char *)malloc(strlen(message) * sizeof(char) + 1);
    //strcpy(this->message, message);
    this->message = message;
}