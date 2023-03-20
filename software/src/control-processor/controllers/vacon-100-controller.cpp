#include <Arduino.h>
#include <errno.h>
#include "vacon-100-controller.h"
#include "logging.h"
#include "binary-util.h"

#define MAX485_RE A3
#define MAX485_DE A2
#define MAX485_RO 9
#define MAX485_DI 8
// Number of consecutive errors beyond which connection to Vacon is considered unavailable
#define MAX_ERROR_COUNT 2

// Time in 2^14 milliseconds between reading values
#define VACON_UPDATE_INTERVAL 4 // ~1 minute
// Whether to start with initial determinate state of off
#define VACON_OFF_ON_STARTUP true
// Whether to force control when Fieldbus is not the configured control place
#define VACON_FORCE_CONTROL false

namespace IrrigationSystem
{
    Vacon100Controller::Vacon100Controller(uint8_t controllerId) : controllerId(controllerId),
                                                                   definition(),
                                                                   serial(MAX485_RO, MAX485_DI),
                                                                   vacon(serial, MAX485_RE, MAX485_DE, MAX485_DI),
                                                                   values(),
                                                                   desiredMotorOn(false),
                                                                   desiredMotorOnIndeterminate(false),
                                                                   lastUpdateTime(0),
                                                                   serialStarted(false),
                                                                   idMapUpdated(false),
                                                                   errorCount(255),
                                                                   eventHandler(nullptr)
    {
    }

    void Vacon100Controller::setEventHandler(EventHandler &handler)
    {
        this->eventHandler = &handler;
    }

    void Vacon100Controller::configure(uint8_t type, const uint8_t *data)
    {
        definition.configure(type, data);
    }

    bool Vacon100Controller::begin()
    {
        if (!serialStarted)
        {
            serial.begin(9600);
            serialStarted = true;
        }
        if (!vacon.begin())
        {
            notifyError(0x00);
            LOG_ERROR("Failed to start Vacon 100 client");
            vacon.printError();
            return false;
        }
        if (!idMapUpdated)
        {
            if (!vacon.initIdMapping())
            {
                notifyError(0x01);
                LOG_ERROR("Failed to set up Vacon 100 ID mappings");
                vacon.printError();
                return false;
            }
            idMapUpdated = true;
        }
        return true;
    }

    void Vacon100Controller::reset()
    {
        vacon.end();
        idMapUpdated = false;
        if (serialStarted)
        {
            serial.end();
            serialStarted = false;
        }
        definition.reset();

        desiredMotorOn = false;
        desiredMotorOnIndeterminate = !VACON_OFF_ON_STARTUP;
        lastUpdateTime = -VACON_UPDATE_INTERVAL - 1;
        errorCount = 255;
        values = {};
    }

    const IrrigationSystem::ControllerDefinition &Vacon100Controller::getDefinition() const
    {
        return definition;
    }

    uint32_t Vacon100Controller::getPropertyValue(uint16_t id) const
    {
        if (id == Vacon100ControllerProperties::available)
        {
            return errorCount <= MAX_ERROR_COUNT;
        }
        else
        {
            return this->getPropertyValueFromValues(values, id);
        }
    }

    uint32_t Vacon100Controller::getPropertyDesiredValue(uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            return desiredMotorOnIndeterminate ? this->getPropertyValueFromValues(values, id) : desiredMotorOn;
        default:
            LOG_ERROR("getPropertyDesiredValue with unknown Vacon 100 property");
            return 0;
        }
    }

    void Vacon100Controller::setPropertyDesiredValue(uint16_t id, uint32_t value)
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            desiredMotorOn = value > 0;
            desiredMotorOnIndeterminate = false;
            if (eventHandler != nullptr)
            {
                eventHandler->handlePropertyDesiredValueChanged(controllerId, id, 1, value);
            }
            break;
        default:
            LOG_ERROR("setPropertyDesiredValue with unknown Vacon 100 property");
            return;
        }
    }

    int Vacon100Controller::runCommand(const uint8_t *input, size_t inputSize, uint8_t *responseOut, size_t *responseSizeOut)
    {
        if (inputSize == 3 && input[0] == 1)
        {
            // Read value
            uint16_t value = vacon.readRaw(read16LE(&input[1]));
            uint16_t error = vacon.getErrorCode();

            // Response is <is error><value | error num>
            if (error == 0)
            {
                responseOut[0] = 0;
                write16LE(&responseOut[1], value);
            }
            else
            {
                responseOut[0] = 1;
                write16LE(&responseOut[1], error);
            }
            *responseSizeOut = 3;
            return 0;
        }
        else if (inputSize == 5 && input[0] == 2)
        {
            // Write value
            // Response is <is error><error num?>
            if (vacon.writeRaw(read16LE(&input[1]), read16LE(&input[3])) == 0)
            {
                responseOut[0] = 0;
                *responseSizeOut = 1;
            }
            else
            {
                responseOut[0] = 1;
                write16LE(&responseOut[1], vacon.getErrorCode());
                *responseSizeOut = 3;
            }
            return 0;
        }
        else
        {
            return 1;
        }
    }

    void Vacon100Controller::update()
    {
        // Read values if update is due or current value doesn't match desired
        uint8_t now = millis() >> 14;
        if (
            (!desiredMotorOnIndeterminate && getPropertyValue(Vacon100ControllerProperties::motorOn) != desiredMotorOn) ||
            (uint8_t)(now - lastUpdateTime) > VACON_UPDATE_INTERVAL)
        {
            Vacon100Data oldValues = values;
            if (vacon.readInputRegisters(&values))
            {
                lastUpdateTime = millis() >> 14;
                updateErrorCount(true);
                if (eventHandler != nullptr)
                {
                    // Raise events for changes to vacon data, all properties except for first one (available)
                    for (unsigned int i = 1; i < definition.getPropertyCount(); ++i)
                    {
                        uint8_t propertyId = definition.getPropertyIdAt(i);
                        uint32_t newValue = getPropertyValueFromValues(values, propertyId);
                        if (newValue != getPropertyValueFromValues(oldValues, propertyId))
                        {
                            eventHandler->handlePropertyValueChanged(controllerId, propertyId, definition.getPropertyLength(propertyId), newValue);
                            if (desiredMotorOnIndeterminate && propertyId == Vacon100ControllerProperties::motorOn)
                            {
                                eventHandler->handlePropertyDesiredValueChanged(controllerId, propertyId, definition.getPropertyLength(propertyId), newValue);
                            }
                        }
                    }
                }
            }
            else
            {
                updateErrorCount(false);
                notifyError(0x03);
                LOG_ERROR("Failed to read from Vacon 100");
                vacon.printError();
            }
        }

        // Write value if current value doesn't match desired
        if (!desiredMotorOnIndeterminate && getPropertyValue(Vacon100ControllerProperties::motorOn) != desiredMotorOn)
        {
            bool successful = false;
            for (int attempt = 0; attempt <= MAX_ERROR_COUNT; ++attempt)
            {
                if (vacon.setStart(desiredMotorOn, VACON_FORCE_CONTROL))
                {
                    successful = true;
                    break;
                }
                else
                {
                    updateErrorCount(false);
                    notifyError(0x02);
                    LOG_ERROR("Failed to write to Vacon 100");
                    vacon.printError();
                }
            }
            if (!successful)
            {
                // Revert desiredMotorOn to match actual value and make it indeterminate
                desiredMotorOn = getPropertyValueFromValues(values, Vacon100ControllerProperties::motorOn);
                desiredMotorOnIndeterminate = true;
                if (eventHandler != nullptr)
                {
                    eventHandler->handlePropertyDesiredValueChanged(controllerId, Vacon100ControllerProperties::motorOn, 1, desiredMotorOn);
                }
            }
        }
    }

    uint32_t Vacon100Controller::getPropertyValueFromValues(const Vacon100Data &values, uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            return (values.statusWord & Vacon100StatusWordMask::run) > 0;
        case Vacon100ControllerProperties::status:
            return values.statusWord;
        case Vacon100ControllerProperties::actualSpeed:
            return values.actualSpeed;
        case Vacon100ControllerProperties::outputFrequency:
            return values.outputFrequency;
        case Vacon100ControllerProperties::motorSpeed:
            return values.motorSpeed;
        case Vacon100ControllerProperties::motorCurrent:
            return values.motorCurrent;
        case Vacon100ControllerProperties::motorTorque:
            return values.motorTorque;
        case Vacon100ControllerProperties::motorPower:
            return values.motorPower;
        case Vacon100ControllerProperties::motorVoltage:
            return values.motorVoltage;
        case Vacon100ControllerProperties::dcLinkVoltage:
            return values.dcLinkVoltage;
        case Vacon100ControllerProperties::activeFaultCode:
            return values.activeFaultCode;
        case Vacon100ControllerProperties::feedbackPressure:
            return values.feedbackPressure;
        case Vacon100ControllerProperties::driveTemp:
            return values.driveTemp;
        case Vacon100ControllerProperties::motorTemp:
            return values.motorTemp;
        case Vacon100ControllerProperties::energyUsed:
            return values.energyUsed;
        case Vacon100ControllerProperties::runTime:
            return values.runTime;
        default:
            LOG_ERROR("getPropertyValue with unknown Vacon 100 property");
            return 0;
        }
    }

    // Increment the error count, or set it back to 0 if reset is true
    void Vacon100Controller::updateErrorCount(bool reset)
    {
        bool previousAvailable = errorCount <= MAX_ERROR_COUNT;
        if (reset)
        {
            errorCount = 0;
        }
        else if (errorCount < 255)
        {
            ++errorCount;
        }
        bool available = errorCount <= MAX_ERROR_COUNT;
        if (previousAvailable != available && eventHandler != nullptr)
        {
            eventHandler->handlePropertyValueChanged(controllerId, Vacon100ControllerProperties::available, 1, available ? 1 : 0);
        }
    }

    void Vacon100Controller::notifyError(uint8_t data)
    {
        if (eventHandler != nullptr)
        {
            uint16_t errorCode = vacon.getErrorCode();
            uint8_t errorPayload[] = {controllerId, data, (uint8_t)errorCode, (uint8_t)(errorCode >> 8)};
            eventHandler->handleEvent(EventType::controllerError, sizeof errorPayload, errorPayload);
        }
    }
}
