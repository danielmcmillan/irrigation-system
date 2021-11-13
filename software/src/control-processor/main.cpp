#include <Arduino.h>
#include "hello.h"

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    hello("Controller Atmega328p");
    delay(1000);
}
