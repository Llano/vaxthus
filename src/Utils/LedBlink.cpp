#include "LedBink.hpp"



LedBlink::LedBlink(int ledPin, int delay) {
    this->ledPin = ledPin;
    this->delay  = delay;
    pinMode(this->ledPin, OUTPUT); 


}

void LedBlink::Update() {
    long currentMillis = millis();

    if(currentMillis - this->lastMillis > this->delay) {

        digitalWrite(this->ledPin, this->state);
		lastMillis = millis();
        this->state = !this->state;

	}

}