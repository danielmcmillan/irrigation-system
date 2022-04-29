#include <Arduino.h>
#include <avr/sleep.h>
#include "yl-800t.h"
#include "serial-interface.h"
#include "config.h"
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

int intCount = 0;

RemoteUnitConfig config;
SolenoidDefinition solenoidDefinitions[] = {
    {DRV_A1, DRV_A2},
    {DRV_B1, DRV_B2}};
Solenoids solenoids(config, solenoidDefinitions);
RemoteUnitBattery battery(config, 0, DISABLE_CHARGE);
RemoteUnitCommandHandler commandHandler(config, solenoids, battery);
RemoteUnitSerialInterface remoteUnitSerial(NODE_ID, commandHandler);

void configureLora()
{
    // Wake up the RF module
    digitalWrite(RF_EN, LOW);
    Serial.begin(9600);
    const uint8_t *rfConfig = config.getRfConfig();
    YL800TReadWriteAllParameters params = {
        .serialBaudRate = YL_800T_BAUD_RATE_9600,
        .serialParity = YL_800T_PARITY_NONE,
        .rfFrequency = (uint32_t)rfConfig[0] << 16 | (uint32_t)rfConfig[1] << 8 | rfConfig[2],
        .rfSpreadingFactor = YL_800T_RF_SPREADING_FACTOR_2048,
        .mode = YL_800T_RF_MODE_NODE,
        .rfBandwidth = YL_800T_RF_BANDWIDTH_125K,
        .nodeId = NODE_ID,
        .netId = 0,
        .rfTransmitPower = rfConfig[3],
        .breathCycle = rfConfig[4],
        .breathTime = rfConfig[5],
        // .rfFrequency = 434l * 1l << 14,
        // .rfTransmitPower = 5,
        // .breathCycle = YL_800T_BREATH_CYCLE_2S,
        // .breathTime = YL_800T_BREATH_TIME_32MS,
    };
    uint8_t message[25] = {0};
    uint8_t length = yl800tSendWriteAllParameters(&params, message);
    delay(100); // TODO why?
    Serial.write(message, length);
    Serial.end();
}

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
    // Turns off modules and puts the processor to sleep until RF is received
    // Based on tips from https://www.gammon.com.au/forum/?id=11497

    // Put RF module to sleep
    digitalWrite(RF_EN, HIGH);
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
    pinMode(RF_EN, OUTPUT);

    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);

    config.loadFromEeprom();
    solenoids.setup();
    battery.setup();
    // Write configuration to LoRa module
    configureLora();

    // Enable pull-up on interrupt pin
    pinMode(2, INPUT_PULLUP);

    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
}

void loop()
{
    unsigned long now = millis();

    // Wake up the RF module
    digitalWrite(RF_EN, LOW);

    // Todo: increase timeout when in sleep mode
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

    delay(500); // TODO why?

    // TODO applying config to lora module
    // TODO reception strength
    // TODO faults
    // TODO auto turn off solenoids
    // TODO temporarily apply RF config change until successful communication

    if (battery.shouldSleep())
    {
        // sleep();
    }
}
