#include <Arduino.h>
#include "vacon-100-controller-definition.h"

namespace IrrigationSystem
{
    void Vacon100ControllerDefinition::reset()
    {
    }

    void Vacon100ControllerDefinition::configure(uint8_t type, const uint8_t *data) {}

    unsigned int Vacon100ControllerDefinition::getConfigLength(uint8_t type) const
    {
        return 0;
    }

    unsigned int Vacon100ControllerDefinition::getPropertyCount() const
    {
        return (sizeof Vacon100ControllerProperties::propertyIds) / (sizeof Vacon100ControllerProperties::propertyIds[0]);
    }

    uint16_t Vacon100ControllerDefinition::getPropertyIdAt(unsigned int index) const
    {
        return Vacon100ControllerProperties::propertyIds[index];
    }

    unsigned int Vacon100ControllerDefinition::getPropertyLength(uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::available:
        case Vacon100ControllerProperties::motorOn:
        case Vacon100ControllerProperties::status:
            return 1;
        case Vacon100ControllerProperties::actualSpeed:
        case Vacon100ControllerProperties::outputFrequency:
        case Vacon100ControllerProperties::motorSpeed:
        case Vacon100ControllerProperties::motorCurrent:
        case Vacon100ControllerProperties::motorTorque:
        case Vacon100ControllerProperties::motorPower:
        case Vacon100ControllerProperties::motorVoltage:
        case Vacon100ControllerProperties::dcLinkVoltage:
        case Vacon100ControllerProperties::activeFaultCode:
        case Vacon100ControllerProperties::feedbackPressure:
        case Vacon100ControllerProperties::driveTemp:
        case Vacon100ControllerProperties::motorTemp:
            return 2;
        case Vacon100ControllerProperties::energyUsed:
        case Vacon100ControllerProperties::runTime:
            return 4;
        default:
            return 0;
        }
    }

    bool Vacon100ControllerDefinition::getPropertyReadOnly(uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            return false;
        default:
            return true;
        }
    }

#ifdef INCLUDE_CONTROLLER_METADATA
    int Vacon100ControllerDefinition::getPropertyIndex(uint16_t id) const
    {
        for (int i = 0; i < sizeof(Vacon100ControllerProperties::propertyIds) / sizeof(Vacon100ControllerProperties::propertyIds[0]); ++i)
        {
            if (Vacon100ControllerProperties::propertyIds[i] == id)
            {
                return i;
            }
        }
        return -1;
    }

    uint8_t Vacon100ControllerDefinition::getName(char *nameOut, uint8_t maxLen) const
    {
        return stpncpy(nameOut, "Vacon 100", maxLen) - nameOut;
    }

    uint8_t Vacon100ControllerDefinition::getPropertyObjectName(uint16_t id, char *nameOut, uint8_t maxLen) const
    {
        return getName(nameOut, maxLen);
    }

    uint8_t Vacon100ControllerDefinition::getPropertyName(uint16_t id, char *nameOut, uint8_t maxLen) const
    {
        static constexpr const char *propertyNames[] = {
            "Available",
            "Motor On",
            "Ready|Run|Direction|Fault|Alarm|At Reference|Zero Speed|Flux Ready",
            "Actual Speed",
            "Output Frequency",
            "Motor Speed",
            "Motor Current",
            "Motor Torque",
            "Motor Power",
            "Motor Voltage",
            "DC Link Voltage",
            "Active Fault Code",
            "Feedback Pressure",
            "Drive Temp",
            "Motor Temp",
            "Energy Used",
            "Run Time"};
        int index = getPropertyIndex(id);
        if (index >= 0)
        {
            return stpncpy(nameOut, propertyNames[index], maxLen) - nameOut;
        }
        else
        {
            return 0;
        }
    }

    PropertyFormat Vacon100ControllerDefinition::getPropertyFormat(uint16_t id) const
    {
        static constexpr PropertyFormat propertyFormats[] = {
            {PropertyValueType::BooleanFlags, {.booleanCount = 1u}},   // available
            {PropertyValueType::BooleanFlags, {.booleanCount = 1u}},   // motorOn
            {PropertyValueType::BooleanFlags, {.booleanCount = 8u}},   // status
            {PropertyValueType::UnsignedInt, {.mul = {10, -2}}, "%"},  // actualSpeed
            {PropertyValueType::UnsignedInt, {.mul = {10, -2}}, "Hz"}, // outputFrequency
            {PropertyValueType::UnsignedInt, {.mul = {1, 1}}, "rpm"},  // motorSpeed
            {PropertyValueType::UnsignedInt, {.mul = {10, -1}}, "A"},  // motorCurrent
            {PropertyValueType::UnsignedInt, {.mul = {10, -1}}, "%"},  // motorTorque
            {PropertyValueType::UnsignedInt, {.mul = {10, -1}}, "%"},  // motorPower
            {PropertyValueType::UnsignedInt, {.mul = {10, -1}}, "V"},  // motorVoltage
            {PropertyValueType::UnsignedInt, {.mul = {1, 1}}, "V"},    // dcLinkVoltage
            {PropertyValueType::UnsignedInt, {.mul = {1, 1}}},         // activeFaultCode
            {PropertyValueType::UnsignedInt, {.mul = {1, 1}}},         // feedbackPressure
            {PropertyValueType::SignedInt, {.mul = {10, -1}}, "C"},    // driveTemp
            {PropertyValueType::UnsignedInt, {.mul = {10, -1}}, "%"},  // motorTemp
            {PropertyValueType::UnsignedInt, {.mul = {1, 1}}, "kWh"},  // energyUsed
            {PropertyValueType::UnsignedInt, {.mul = {60, -2}}, "h"}   // runTime
        };
        int index = getPropertyIndex(id);
        if (index >= 0)
        {
            return propertyFormats[index];
        }
        else
        {
            return {};
        }
    }
#endif
}
