#ifndef _CONTROLLER_DEFINITION_H
#define _CONTROLLER_DEFINITION_H
#include <inttypes.h>

namespace IrrigationSystem
{
    enum class PropertyValueType : uint8_t
    {
        BooleanFlags,
        UnsignedInt,
        SignedInt
    };

    struct PropertyFormat
    {
        PropertyValueType valueType;
        union
        {
            uint8_t booleanCount;
            struct
            {
                uint8_t base;
                int8_t exponent;
            } mul;
        } options;
        const char *unit;
    };

    /** Provides information about a controller given a set of configuration. */
    class ControllerDefinition
    {
    public:
        virtual void reset() = 0;
        virtual void configure(uint8_t type, const uint8_t *data) = 0;
        /**
         * Get the expected length of configuration data for the specified type.
         */
        virtual unsigned int getConfigLength(uint8_t type) const = 0;
        /**
         * Get the number of available properties.
         */
        virtual unsigned int getPropertyCount() const = 0;
        /**
         * Get the id of the property at the given index, assuming some ordering of properties.
         * The order of properties is irrelevant, so long as it is consistent.
         */
        virtual uint16_t getPropertyIdAt(unsigned int index) const = 0;
        /**
         * Get the length of the property with the specified id in bytes.
         *
         * Returns 0 if the property does not exist.
         */
        virtual unsigned int getPropertyLength(uint16_t id) const = 0;
        /**
         * Returns true if the property with the specified id is read-only.
         * Otherwise returns false.
         */
        virtual bool getPropertyReadOnly(uint16_t id) const = 0;

#ifdef INCLUDE_CONTROLLER_METADATA
        /**
         * Retrieve the display name for the controller.
         * @param nameOut Buffer where the string will be written to. It may or may not get null terminated.
         * @param maxLen Maximum length to write to nameOut including null terminator.
         * @returns The length of the string written to nameOut excluding null terminator.
         */
        virtual uint8_t getName(char *nameOut, uint8_t maxLen) const = 0;
        /**
         * Retrieve the name of the object that the property is for.
         */
        virtual uint8_t getPropertyObjectName(uint16_t id, char *nameOut, uint8_t maxLen) const = 0;
        /**
         * Retrieve the name for the property with the specified id.
         * For a boolean flag property, each part can be named by separating with | character.
         */
        virtual uint8_t getPropertyName(uint16_t id, char *nameOut, uint8_t maxLen) const = 0;
        /**
         * Returns information about the property value format.
         */
        virtual PropertyFormat getPropertyFormat(uint16_t id) const = 0;
#endif
    };
}
#endif
