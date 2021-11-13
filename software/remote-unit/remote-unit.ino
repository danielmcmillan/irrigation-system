// Turns off modules and puts the processor to sleep until RF is received
// Based on tips from https://www.gammon.com.au/forum/?id=11497

// Note: add __AVR_ATmega328P__ to defined in c_cpp_properties.json

#include <avr/sleep.h>

// Pins
#define RF_EN 4
#define DRV_A1 9
#define DRV_A2 10
#define DRV_B1 5
#define DRV_B2 6
#define LED_1 8
#define LED_2 A1

int intCount = 0;

void wake()
{
    // cancel sleep as a precaution
    sleep_disable();
    // precautionary while we do other stuff
    detachInterrupt(0);
    ++intCount;
    digitalWrite(RF_EN, LOW); // warning maybe concurrency issue if writing something different in loop
}

void sleep()
{
    // Put RF module to sleep
    digitalWrite(RF_EN, HIGH);
    // Put motor drivers to sleep
    digitalWrite(DRV_A1, LOW);
    digitalWrite(DRV_A2, LOW);
    digitalWrite(DRV_B1, LOW);
    digitalWrite(DRV_B2, LOW);

    // disable ADC
    //  ADCSRA = 0;

    // Disable processor modules (no different for PWR_DOWN)
    //  power_all_disable();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Do not interrupt before we go to sleep, or the
    // ISR will detach interrupts and we won't wake.
    noInterrupts();

    // will be called when pin D2 goes low
    attachInterrupt(0, wake, FALLING);
    EIFR = bit(INTF0); // clear flag for interrupt 0

    // turn off brown-out enable in software
    // BODS must be set to one and BODSE must be set to zero within four clock cycles
    MCUCR = bit(BODS) | bit(BODSE);
    // The BODS bit is automatically cleared after three clock cycles
    MCUCR = bit(BODS);

    // We are guaranteed that the sleep_cpu call will be done
    // as the processor executes the next instruction after
    // interrupts are turned on.
    interrupts(); // one cycle

    sleep_cpu();
}

void setup()
{
    pinMode(RF_EN, OUTPUT);
    pinMode(DRV_A1, OUTPUT);
    pinMode(DRV_A2, OUTPUT);
    pinMode(DRV_B1, OUTPUT);
    pinMode(DRV_B2, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);

    analogReference(INTERNAL);

    // Enable pull-up on interrupt pin
    pinMode(2, INPUT_PULLUP);

    digitalWrite(DRV_A1, LOW);
    digitalWrite(DRV_A2, LOW);
    digitalWrite(DRV_B1, LOW);
    digitalWrite(DRV_B2, LOW);
}

char inputBuffer[256] = {0};
char *inputPointer = inputBuffer;

void loop()
{
    digitalWrite(LED_2, HIGH);

    // Wake up the RF module
    digitalWrite(RF_EN, LOW);

    // Read incoming data from RF module
    Serial.begin(9600);
    unsigned long lastReadTime = millis();
    inputPointer = inputBuffer;
    // TODO proper serial read, work out timing
    while (Serial.available() || millis() < lastReadTime + 500)
    {
        int b = Serial.read();
        if (b >= 0)
        {
            *inputPointer = b;
            ++inputPointer;
            lastReadTime = millis();
        }
    }

    if (inputPointer > inputBuffer)
    {
        char *token = inputBuffer;
        for (char *i = inputBuffer; i < inputPointer; ++i)
        {
            if (*i == '\n')
            {
                if (strncmp(token, "LON", i - token) == 0)
                {
                    digitalWrite(LED_1, HIGH);
                    Serial.println("OK LON");
                }
                else if (strncmp(token, "LOFF", i - token) == 0)
                {
                    digitalWrite(LED_1, LOW);
                    Serial.println("OK LOFF");
                }
                else if (strncmp(token, "BAT", i - token) == 0)
                {
                    analogRead(0);
                    float av = analogRead(0);
                    Serial.print("OK BAT ");
                    Serial.print(av);
                    Serial.print(" (");
                    Serial.print(av * 0.015);
                    Serial.println("V)");
                }
                else if (strncmp(token, "SOLON", i - token) == 0)
                {
                    digitalWrite(DRV_A1, HIGH);
                    delay(100);
                    digitalWrite(DRV_A1, LOW);
                    Serial.println("OK SOLON");
                }
                else if (strncmp(token, "SOLOFF", i - token) == 0)
                {
                    digitalWrite(DRV_A2, HIGH);
                    delay(100);
                    digitalWrite(DRV_A2, LOW);
                    Serial.println("OK SOLOFF");
                }
                else
                {
                    Serial.println("ERR");
                }
                token = i + 1;
            }
        }
    }

    // analogRead(0);
    // float av = analogRead(0);
    // delay(50);
    // Serial.print("Interrupt count: ");
    // Serial.println(intCount);
    // Serial.print("Analog value: ");
    // Serial.println(av);
    // Check for data to read from RF module
    // unsigned long startTime = millis();
    // while (Serial.available() || millis() < startTime + 2000)
    // {
    //   digitalWrite(LED_2, HIGH);
    //   String s = Serial.readString();
    //   if (s.length() > 0)
    //   {
    //     Serial.print("ECHO: ");
    //     Serial.println(s);
    //   }
    // }
    // digitalWrite(LED_2, LOW);

    Serial.end();
    delay(500);

    digitalWrite(LED_2, LOW);
    sleep();
}
