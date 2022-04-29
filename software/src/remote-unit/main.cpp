#include <Arduino.h>
#include <avr/sleep.h>
#include "yl-800t.h"
#include "serial-interface.h"
#include "config.h"
#include "rf-module.h"
#include "solenoids.h"
#include "battery.h"

/**
 * Unique 16 bit identifier for a remote unit.
 * Each remote unit should have a unique value.
 */
#define NODE_ID 0x00ff

// Pins
#define RF_EN 4
#define LED_1 8
#define LED_2 A1

// Solenoid driver pins
#define DRV_A1 9
#define DRV_A2 10
#define DRV_B1 5
#define DRV_B2 6

// Pulled low to enable solar charge
#define DISABLE_CHARGE A4

unsigned long lastSuccessfulCommunication = 0;

RemoteUnitConfig config;
SolenoidDefinition solenoidDefinitions[] = {
    {DRV_A1, DRV_A2},
    {DRV_B1, DRV_B2}};
RemoteUnitRfModule rfModule(NODE_ID, config, RF_EN);
Solenoids solenoids(config, solenoidDefinitions);
RemoteUnitBattery battery(config, 0, DISABLE_CHARGE);
RemoteUnitCommandHandler commandHandler(config, rfModule, solenoids, battery);
RemoteUnitSerialInterface remoteUnitSerial(NODE_ID, commandHandler);

void wake()
{
    // cancel sleep as a precaution
    sleep_disable();
    // precautionary while we do other stuff
    detachInterrupt(0);
    rfModule.wake(); // warning maybe concurrency issue if writing something different in loop
}

void sleep()
{
    // Turns off modules and puts the processor to sleep until RF is received
    // Based on tips from https://www.gammon.com.au/forum/?id=11497

    Serial.end();
    rfModule.sleep();
    // Put motor drivers to sleep
    solenoids.sleep();

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
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);

    config.loadFromEeprom();
    rfModule.setup();
    solenoids.setup();
    battery.setup();
    rfModule.applyConfig();

    // Enable pull-up on interrupt pin
    pinMode(2, INPUT_PULLUP);

    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
}

void loop()
{
    Serial.begin(9600, SERIAL_8N1);
    unsigned long now = millis();
    rfModule.wake();

    // Todo: increase timeout when in sleep mode since we are expecting data
    RemoteUnitSerialInterface::Result result = remoteUnitSerial.receivePacket(500);

    // Flash to show error
    if (result != RemoteUnitSerialInterface::Result::success && result != RemoteUnitSerialInterface::Result::noData)
    {
        for (int i = 0; i < (uint8_t)result; ++i)
        {
            digitalWrite(LED_1, HIGH);
            delay(500);
            digitalWrite(LED_1, LOW);
            delay(500);
        }
    }

    battery.update(now);

    if (result == RemoteUnitSerialInterface::Result::success)
    {
        lastSuccessfulCommunication = now;
    }
    else if (now - lastSuccessfulCommunication > config.getSolenoidTimeout() << 4)
    {
        // No successful communication received for the configured timeout, ensure valves are shut off
        solenoids.setState(0);
    }

    delay(500); // TODO why?

    // TODO faults
    // TODO temporarily apply RF config change until successful communication

    if (battery.shouldSleep())
    {
        // sleep();
    }
}
