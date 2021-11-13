#include <Arduino.h>
#include "hello.h"

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    hello("ESP32");
    delay(1000);
}
