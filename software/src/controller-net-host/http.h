#ifndef NET_HOST_HTTP_H
#define NET_HOST_HTTP_H
/**
 * Http server for testing
 */

#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "ControlI2CHost.h"

ControlI2CHost cih;
AsyncWebServer server(80);

const char *http_username = "admin";
const char *http_password = "admin";

// enum class PropertyFormat
// {
//     boolean,
//     signedInt,
//     unsignedInt
// };
struct Property
{
    uint8_t controller;
    uint16_t id;
    /** The number of bytes for the value */
    uint8_t size;
    bool readOnly;
    // /** Power of 10 that the value is multiplied by */
    // int8_t mul;
    // /** The format of the data */
    // PropertyFormat format;
    /** Property value, little-endian */
    uint8_t value[4];
    /** For a writeable property, the selected value for the property */
    uint8_t desiredValue[4];
};

// Motor on property
Property properties[] = {{.controller = 0x02, .id = 0x0002, .size = 1, .readOnly = false}};

void logProperties()
{
    for (Property property : properties)
    {
        sprintf(logBuffer, "Controller 0x%02x, id 0x%04x, value 0x%02x, desired 0x%02x", property.controller, property.id, property.value[0], property.desiredValue[0]);
        Serial.println(logBuffer);
    }
}
// cih.getPropertyValue(properties[0].controller, properties[0].id, properties[0].size, properties[0].readOnly, properties[0].value, properties[0].desiredValue);
// logProperties();
// delay(2000);

void onRequest(AsyncWebServerRequest *request)
{
    if (request->method() == WebRequestMethod::HTTP_OPTIONS)
    {
        AsyncWebServerResponse *response = request->beginResponse(200);
        sprintf(logBuffer, "Received preflight for %s %s", request->methodToString(), request->url().c_str());
        Serial.println(logBuffer);
        response->addHeader("Allow", "OPTIONS, GET, HEAD, POST");
        request->send(response);
    }
    else
    {
        // Handle Unknown Request
        sprintf(logBuffer, "404 %s %s", request->methodToString(), request->url().c_str());
        Serial.println(logBuffer);
        request->send(404);
    }
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    // Handle body
}

uint8_t inBuffer[128];
uint8_t outBuffer[128];
char *charBuffer = (char *)inBuffer;

void setupHttp(IrrigationSystem::ControllerDefinitionManager &definitions)
{
    cih.begin();
    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
              {
            sprintf(charBuffer, "{\"free\":%lu}", ESP.getFreeHeap());
                request->send(200, "application/json", charBuffer); });

    server.on(
        "/raw",
        HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            AsyncWebParameter *dataParam = request->getParam("d");
            if (dataParam != nullptr)
            {
                size_t dataSize = dataParam->value().length() / 2;
                for (int i = 0; i < dataSize; ++i)
                {
                    inBuffer[i] = strtoul(dataParam->value().substring(i * 2, i * 2 + 2).c_str(), nullptr, 16);
                }

                size_t responseSize;
                int result = cih.sendRawData(inBuffer, dataSize, &responseSize, outBuffer);
                char *body = logBuffer;

                body += sprintf(body, "Request (%d bytes): 0x", dataSize);
                for (int i = 0; i < dataSize; ++i)
                {
                    body += sprintf(body, "%02x", inBuffer[i]);
                }
                body += sprintf(body, "\nResponse (%d bytes): 0x", responseSize);
                for (int i = 0; i < responseSize; ++i)
                {
                    body += sprintf(body, "%02x", outBuffer[i]);
                }
                body += sprintf(body, "\n");
                if (result == 0)
                {
                    request->send(200, "text/plain", logBuffer);
                }
                else
                {
                    body += sprintf(body, "Error: %d\n", result);
                    request->send(500, "text/plain", logBuffer);
                }
            }
            else
            {
                request->send(400, "text/plain", "Invalid parameters");
            }
        });

    server.on("/api/getProperty", HTTP_GET, [definitions](AsyncWebServerRequest *request)
              {
        String controllerParam = request->getParam("controller")->value();
        String idParam = request->getParam("id")->value();
        uint8_t controllerId = strtoul(controllerParam.c_str(), NULL, 16);
        uint16_t propertyId = strtoul(idParam.c_str(), NULL, 16);
        const IrrigationSystem::ControllerDefinition *definition = definitions.getControllerDefinition(controllerId);
        uint8_t valueParts[4];
        uint8_t desiredValueParts[4];
        bool readOnly = definition->getPropertyReadOnly(propertyId);
        unsigned int valueSize = definition->getPropertyLength(propertyId);
        int result = cih.getPropertyValue(
            controllerId,
            propertyId,
            valueSize,
            readOnly,
            valueParts,
            desiredValueParts); // TODO send i2c request asynchronously

        if (result == 0)
        {
            uint32_t value = 0;
            uint32_t desiredValue = 0;
            for (int i = 0; i < valueSize; ++i)
            {
                value += (uint32_t)valueParts[i] << (8 * i);
                if (!readOnly)
                {
                    desiredValue += (uint32_t)desiredValueParts[i] << (8 * i);
                }
            }

            char *ptr = charBuffer;
            ptr += sprintf(ptr, "{\"value\":%d", value);
            if (!readOnly)
            {
                ptr += sprintf(ptr, ",\"desired\":%d", desiredValue);
            }
            ptr += sprintf(ptr, "}", desiredValue);
            request->send(200, "application/json", charBuffer);
        }
        else
        {
            sprintf(charBuffer, "{\"error\":%d}", result);
            request->send(500, "application/json", charBuffer);
        } });

    server.on("/api/setProperty", HTTP_POST, [definitions](AsyncWebServerRequest *request)
              {
        String controllerParam = request->getParam("controller")->value();
        String idParam = request->getParam("id")->value();
        String valueParam = request->getParam("value")->value();
        uint8_t controllerId = strtoul(controllerParam.c_str(), NULL, 16);
        uint16_t propertyId = strtoul(idParam.c_str(), NULL, 16);
        uint32_t value = strtoul(valueParam.c_str(), NULL, 16);
        uint8_t valueLength = definitions.getControllerDefinition(controllerId)->getPropertyLength(propertyId);
        uint8_t *valueParts = (uint8_t *)&value;                                        // assumes little endian (value is array of bytes with LSB first)
        int result = cih.setPropertyDesiredValue(controllerId, propertyId, valueLength, valueParts); // TODO send i2c request asynchronously

        if (result == 0)
        {
            sprintf(charBuffer, "{\"desired\":%d}", value);
            request->send(200, "application/json", charBuffer);
        }
        else
        {
            sprintf(charBuffer, "{\"error\":%d}", result);
            request->send(500, "application/json", charBuffer);
        } });

    server.on("/api/getNextEvent", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        AsyncWebParameter *afterParam = request->getParam("after");
        String afterParamStr = afterParam != nullptr ? afterParam->value() : String();
        uint16_t afterId = afterParamStr.isEmpty() ? 0 : strtoul(afterParamStr.c_str(), NULL, 10);
        EventHistoryRecord event;
        int result = cih.getNextEvent(afterId, &event);

        if (result <= 0)
        {
            if (result == -1)
            {
                // No events
                sprintf(charBuffer, "{}");
                request->send(200, "application/json", charBuffer);
            }
            else
            {
                char *outPtr = charBuffer;
                outPtr += sprintf(outPtr, "{\"id\":%lu,\"type\":%u,\"found\":%s", event.id, event.type, result == -2 ? "false" : "true");

                int dataPos = -1;
                if (event.type == 0x40 || event.type == 0x80 || event.type == 0xc0)
                {
                    // Events with generic data payload
                    dataPos = 0;
                }
                else if (event.type == 0xc1)
                {
                    // Events with data payload after controller id
                    dataPos = 1;
                }
                else if (event.type == 0xc2)
                {
                    // Events with data payload after controller id and property id
                    dataPos = 2;
                }

                if (event.type == 0x48 || event.type == 0x49 || event.type == 0xc1 || event.type == 0xc2)
                {
                    // Events with controller id
                    outPtr += sprintf(outPtr, ",\"controllerId\":%u", event.payload[0]);
                }
                if (event.type == 0x48 || event.type == 0x49 || event.type == 0xc2)
                {
                    // Events with property id
                    outPtr += sprintf(outPtr, ",\"propertyId\":%u", read16LE(event.payload + 1));
                }
                if (event.type == 0x48 || event.type == 0x49)
                {
                    // Events with property value
                    uint32_t value = 0;
                    for (int i = 0; i < event.payloadSize - 3; ++i)
                    {
                        value |= (uint32_t)event.payload[i + 3] << (i * 8);
                    }
                    outPtr += sprintf(outPtr, ",\"value\":%lu", value);
                }

                if (dataPos >= 0)
                {
                    outPtr += sprintf(outPtr, ",\"data\":\"0x");
                    for (int i = dataPos; i < event.payloadSize; ++i)
                    {
                        outPtr += sprintf(outPtr, "%02x", event.payload[i]);
                    }
                    outPtr += sprintf(outPtr, "\"");
                }

                outPtr += sprintf(outPtr, "}");
                request->send(200, "application/json", charBuffer);
            }
        }
        else
        {
            sprintf(charBuffer, "{\"error\":%d}", result);
            request->send(500, "application/json", charBuffer);
        } });

    server.onNotFound(onRequest);
    server.onRequestBody(onBody);

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");
    server.begin();
}

#endif
