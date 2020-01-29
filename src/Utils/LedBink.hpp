#include "Arduino.h"
class LedBlink {
    public:
        LedBlink(int ledPin, int delay);
        void Update();
    private:
        unsigned long lastMillis = 0;
        int delay = 0;
        int ledPin = 0;
        int state = 0;

};