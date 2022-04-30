#include <Arduino.h>
#include <avr/sleep.h>
#include "yl-800t.h"
#include "serial-interface.h"
#include "config.h"
#include "rf-module.h"
#include "solenoids.h"
#include "battery.h"
#include "faults.h"

/**
 * Unique 16 bit identifier for a remote unit.
 * Each remote unit should have a unique value.
 */
#define NODE_ID 0x00ff

// Pins
#define RF_EN 4
#define LED_1 8
#define LED_2 A1
#define RF_INTERRUPT_PIN 2
#define RF_INTERRUPT_NUMBER 0

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
RemoteUnitFaults faults;
RemoteUnitCommandHandler commandHandler(config, rfModule, solenoids, battery, faults);
RemoteUnitSerialInterface remoteUnitSerial(NODE_ID, commandHandler);

volatile bool dataPending = false;
volatile unsigned long millisApprox = 0;
volatile unsigned long lastCountMillis = 0;

void handleRfModuleInterrupt()
{
    dataPending = true;

    // cancel sleep as a precaution
    // sleep_disable();

    // precautionary while we do other stuff
    // detachInterrupt(0); // TODO remove

    // We need to wake up the RF module as quickly as possible.
    // Warning - maybe concurrency issue if writing something different in loop.
    rfModule.wake();
}

// Watchdog timer interrupt handler
ISR(WDT_vect)
{
    millisApprox += 8000;
    lastCountMillis = millis();
}

void enableTimerInterrupt()
{
    // Watchdog timer interrupt
    // Clear the WD reset flag.
    MCUSR &= ~(1 << WDRF);

    // Set WD change enable and WD system reset enable
    WDTCSR |= (1 << WDCE) | (1 << WDE);

    // Configure the WD prescaler, unset WDE
    WDTCSR = 1 << WDP0 | 1 << WDP3; // 8 seconds

    // Enable the WD interrupt
    WDTCSR |= 1 << WDIE;
}

void enableRfModuleInterrupt()
{
    // Enable pull-up on interrupt pin
    pinMode(RF_INTERRUPT_PIN, INPUT_PULLUP);
    noInterrupts();
    attachInterrupt(RF_INTERRUPT_NUMBER, handleRfModuleInterrupt, FALLING);
    interrupts();
}

void sleep()
{
    // Turns off modules and puts the processor to sleep until RF is received
    // Based on tips from https://www.gammon.com.au/forum/?id=11497
    rfModule.sleep();
    // Put motor drivers to sleep
    solenoids.sleep();

    // Disable ADC - this caused problems
    // ADCSRA = 0;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Call handler when interrupt pin goes low
    // attachInterrupt(0, handleRfModuleInterrupt, FALLING);
    // EIFR = bit(INTF0); // clear flag for interrupt 0

    noInterrupts();
    // enableTimerInterrupt();
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
    // Continues from here after wake
    sleep_disable();
    // noInterrupts();
    // disableTimerInterrupt();
    // interrupts();
}

void setup()
{
    Serial.begin(9600, SERIAL_8N1);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);

    if (config.load())
    {
        faults.setFault(RemoteUnitFault::ConfigReadFailed);
    }
    rfModule.setup();
    solenoids.setup();
    battery.setup();
    rfModule.applyConfig();

    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);

    enableRfModuleInterrupt();
    noInterrupts();
    enableTimerInterrupt();
    interrupts();
}

void loop()
{
    bool handleSerialData = dataPending;
    if (handleSerialData)
    {
        rfModule.wake();
        digitalWrite(LED_2, HIGH);
        Serial.begin(9600, SERIAL_8N1);
    }
    // Approximation of the time elapsed
    unsigned long now = millisApprox + (millis() - lastCountMillis);
    digitalWrite(LED_1, HIGH);
    if (battery.update(now))
    {
        faults.setFault(RemoteUnitFault::BatteryVoltageError);
    }

    if (handleSerialData)
    {
        delay(200);
        // Read a packet from Serial and perform any encoded commands.
        RemoteUnitSerialInterface::Result result = remoteUnitSerial.receivePacket(5000);

        // Flash to show error
        if (result != RemoteUnitSerialInterface::Result::success && result != RemoteUnitSerialInterface::Result::noData)
        {
            digitalWrite(LED_2, LOW);
            delay(300);
            for (int i = 0; i < (uint8_t)result; ++i)
            {
                digitalWrite(LED_2, HIGH);
                delay(300);
                digitalWrite(LED_2, LOW);
                delay(300);
            }
        }
        if (result == RemoteUnitSerialInterface::Result::success)
        {
            lastSuccessfulCommunication = now;
        }
    }

    if (now - lastSuccessfulCommunication > ((unsigned long)(config.getCommunicationTimeout()) << 14))
    {
        // No successful communication received for the configured timeout
        // Revert non-persisted config
        if (config.load())
        {
            faults.setFault(RemoteUnitFault::ConfigReadFailed);
        }
        // Ensure valves are shut off
        if ((solenoids.getState() & ~SOLENOID_FORCE_FLAG) != 0)
        {
            faults.setFault(RemoteUnitFault::SolenoidTimeoutOccurred);
            solenoids.setState(0);
        }
    }
    if (handleSerialData)
    {
        delay(200);
        dataPending = Serial.available() > 0;
    }
    if (!dataPending)
    {
        digitalWrite(LED_2, LOW);
        Serial.end();
        if (!battery.shouldMaintain())
        {
            digitalWrite(LED_1, LOW);
            millisApprox = millisApprox + (millis() - lastCountMillis);
            sleep();
        }
    }
}
