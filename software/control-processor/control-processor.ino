#include <Wire.h>

#define I2C_SLAVE_ADDRESS 10

void setup()
{
    pinMode(6, OUTPUT);
    pinMode(A4, INPUT_PULLUP);
    pinMode(A5, INPUT_PULLUP);
    Serial.begin(9600);
    Wire.begin(I2C_SLAVE_ADDRESS);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    digitalWrite(6, LOW);
}

String serialReceived;

void loop()
{
    if (Serial.available())
    {
        serialReceived = Serial.readString();
    }
}

void receiveEvent(int bytes)
{
    while (Wire.available())
    {
        int b = Wire.read();
        Serial.write(b);
    }
}

void requestEvent()
{
    Wire.print("Ok ");
    Wire.println(serialReceived);
}
