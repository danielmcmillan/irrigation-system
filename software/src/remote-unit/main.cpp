#include <Arduino.h>
#include <avr/sleep.h>
#include "yl-800t.h"
#include "serial-interface.h"
#include "config.h"
#include "rf-module.h"
#include "solenoids.h"
#include "battery.h"
#include "faults.h"
#include "sensor.h"
#include "settings.h"

#define SERIAL_BEGIN Serial.begin(9600, SERIAL_8N1)

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

// RS485 sensor pins
#define RS_485_RX_PIN A2    // PC2
#define RS_485_TX_PIN 3     // PD3
#define RS_485_RE_DE_PIN A5 // SCL
#define RS485_ENABLE_PIN A3 // PC3

unsigned long lastSuccessfulCommunicationCounts = 0;
volatile bool dataPending = false;
// Approximation of time elapsed, multiples of 8 seconds
volatile unsigned long counts = 0;

RemoteUnitConfig config;
SolenoidDefinition solenoidDefinitions[] = {
    {DRV_A1, DRV_A2},
    {DRV_B1, DRV_B2}};
RemoteUnitRfModule rfModule(NODE_ID, config, RF_EN);
Solenoids solenoids(config, solenoidDefinitions);
RemoteUnitBattery battery(config, 0, DISABLE_CHARGE);
RemoteUnitFaults faults;
RemoteUnitSensor sensor(config, RS_485_RX_PIN, RS_485_TX_PIN, RS_485_RE_DE_PIN, RS485_ENABLE_PIN);
RemoteUnitCommandHandler commandHandler(config, rfModule, solenoids, battery, faults, sensor, counts);
RemoteUnitSerialInterface remoteUnitSerial(NODE_ID, commandHandler);

void handleRfModuleInterrupt()
{
    dataPending = true;

    // We need to wake up the RF module as quickly as possible.
    // Warning - maybe concurrency issue if writing something different in loop.
    rfModule.wake();
}

// Watchdog timer interrupt handler
ISR(WDT_vect)
{
    ++counts;
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
    attachInterrupt(RF_INTERRUPT_NUMBER, handleRfModuleInterrupt, FALLING);
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

    // turn off brown-out enable in software
    noInterrupts();
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
}

void setup()
{
    SERIAL_BEGIN;
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    // Startup LED flashing sequence
    for (int i = 0; i < 4; ++i)
    {
        digitalWrite(LED_1, HIGH);
        digitalWrite(LED_2, LOW);
        delay(150);
        digitalWrite(LED_1, LOW);
        digitalWrite(LED_2, HIGH);
        delay(150);
    }
    digitalWrite(LED_2, LOW);

    if (config.load())
    {
        faults.setFault(RemoteUnitFault::ConfigReadFailed);
    }
    rfModule.setup();
    solenoids.setup();
    battery.setup();
    sensor.setup();
    if (rfModule.applyConfig())
    {
        faults.setFault(RemoteUnitFault::ConfigureRfModuleFailed);
    }

    noInterrupts();
    enableRfModuleInterrupt();
    enableTimerInterrupt();
    interrupts();
}

void loop()
{
    bool handleSerialData = dataPending;
    digitalWrite(LED_1, HIGH);
    if (handleSerialData)
    {
        rfModule.wake();
        SERIAL_BEGIN;
    }
    if (battery.update(counts))
    {
        faults.setFault(RemoteUnitFault::BatteryVoltageError);
    }

    if (handleSerialData)
    {
        digitalWrite(LED_2, HIGH);
        // Read a packet from Serial and perform any encoded commands.
        RemoteUnitSerialInterface::Result result = remoteUnitSerial.receivePacket(5000);
        digitalWrite(LED_2, LOW);

        // Flash to show error
        if (result != RemoteUnitSerialInterface::Result::success && result != RemoteUnitSerialInterface::Result::noData)
        {
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
            lastSuccessfulCommunicationCounts = counts;
        }
    }

    if (counts - lastSuccessfulCommunicationCounts > ((unsigned long)(config.getCommunicationTimeout())))
    {
        // No successful communication received for the configured timeout
        // Revert non-persisted config
        if (config.getIsChanged())
        {
            SERIAL_BEGIN;
            if (config.load())
            {
                faults.setFault(RemoteUnitFault::ConfigReadFailed);
            }
            if (rfModule.applyConfig())
            {
                faults.setFault(RemoteUnitFault::ConfigureRfModuleFailed);
            }
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
        // Allow some extra time for RF data to be sent before sleep
        delay(34);
        dataPending = Serial.available() > 0;
    }
    if (!dataPending)
    {
        Serial.end();
        digitalWrite(LED_1, LOW);
        sleep();
    }
}
