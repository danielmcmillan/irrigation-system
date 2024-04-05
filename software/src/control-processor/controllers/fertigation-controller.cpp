#include <Arduino.h>
#include <errno.h>
#include "fertigation-controller.h"
#include "logging.h"
#include "binary-util.h"

#define FERTIGATION_FLOW_SENSOR_PIN 18
#define FLOW_RATE_CALCULATION_PERIOD 30000

static portMUX_TYPE flowSensorCountLock = portMUX_INITIALIZER_UNLOCKED;
volatile uint16_t flowSensorCount = 0;
unsigned long lastSensorCount = 0;
void IRAM_ATTR flowSensorIsr()
{
    unsigned long time = millis();
    if (time - lastSensorCount > 200)
    {
        ++flowSensorCount;
        lastSensorCount = time;
    }
}
void resetFlowSensorCount()
{
    taskENTER_CRITICAL(&flowSensorCountLock);
    flowSensorCount = 0;
    taskEXIT_CRITICAL(&flowSensorCountLock);
}

namespace IrrigationSystem
{
    FertigationController::FertigationController(uint8_t controllerId) : controllerId(controllerId),
                                                                         definition(),
                                                                         eventHandler(nullptr)
    {
    }

    void FertigationController::setEventHandler(EventHandler &handler)
    {
        this->eventHandler = &handler;
    }

    void FertigationController::configure(uint8_t type, const uint8_t *data)
    {
        definition.configure(type, data);
    }

    bool FertigationController::begin()
    {
        pinMode(FERTIGATION_FLOW_SENSOR_PIN, INPUT_PULLUP);
        attachInterrupt(FERTIGATION_FLOW_SENSOR_PIN, flowSensorIsr, FALLING);
        return true;
    }

    void FertigationController::reset()
    {
        definition.reset();
        resetFlowSensorCount();
        lastFlowRateCalculationMillis = millis();
        flowRate = 0xffff;
    }

    const IrrigationSystem::ControllerDefinition &FertigationController::getDefinition() const
    {
        return definition;
    }

    bool FertigationController::getPropertyValue(uint16_t id, uint32_t *value) const
    {
        if (id == FertigationControllerProperties::flowRate)
        {
            *value = flowRate;
            return flowRate != 0xffff;
        }
        return false;
    }

    uint32_t FertigationController::getPropertyDesiredValue(uint16_t id) const
    {
        return 0;
    }

    void FertigationController::setPropertyDesiredValue(uint16_t id, uint32_t value)
    {
        return;
    }

    uint16_t FertigationController::runCommand(const uint8_t *input, size_t inputSize, uint8_t *responseOut, size_t *responseSizeOut)
    {
        if (inputSize == 1 && input[0] == 1)
        {
            write16LE(responseOut, flowSensorCount);
            *responseSizeOut = 2;
            return 0;
        }
        else
        {
            return 1;
        }
    }

    void FertigationController::update()
    {
        unsigned long timeSinceLastFlowRateCalculation = millis() - lastFlowRateCalculationMillis;
        if (definition.enable && timeSinceLastFlowRateCalculation >= FLOW_RATE_CALCULATION_PERIOD)
        {
            uint16_t prevFlowRate = flowRate;
            flowRate = (uint32_t)flowSensorCount * (10000000 / (uint32_t)timeSinceLastFlowRateCalculation);
            resetFlowSensorCount();
            lastFlowRateCalculationMillis = millis();

            if (eventHandler != nullptr && flowRate != prevFlowRate)
            {
                eventHandler->handlePropertyValueChanged(controllerId, FertigationControllerProperties::flowRate, 2, flowRate);
            }
        }
    }
}
