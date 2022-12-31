#include <Arduino.h>
#include "crc16.h"
#include <Wire.h>
#include "ControlI2CHost.h"
#include "logging.h"
#include "vacon-100-controller-definition.h"
#include "controller-definition-manager.h"

#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
AsyncWebServer server(80);
ControlI2CHost cih;

const char *ssid = "*";
const char *password = "*";
const char *http_username = "admin";
const char *http_password = "admin";

IrrigationSystem::Vacon100ControllerDefinition vacon100Definition;
IrrigationSystem::ControllerDefinitionRegistration registeredDefinitions[] = {
    {0x02, &vacon100Definition}};

IrrigationSystem::ControllerDefinitionManager controllers(registeredDefinitions, sizeof(registeredDefinitions) / sizeof(registeredDefinitions[0]));

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

void onRequest(AsyncWebServerRequest *request)
{
    // Handle Unknown Request
    request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    // Handle body
}

uint8_t inBuffer[128];
uint8_t outBuffer[128];

void setup()
{
    Serial.begin(9600);
    cih.begin();

    // WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("esp32-irrigation");
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("\nWiFi Failed!\n");
        return;
    }
    Serial.printf("\nWiFi Connected: %s\n", WiFi.localIP().toString().c_str());

    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", String(ESP.getFreeHeap())); });

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

    server.on("/properties", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  char response[1024] = {0};
                  sprintf(response,
                          "<html>"
                          "<meta http-equiv=\"refresh\" content=\"5\">"
                          "<body><ul>\n"
                          "<li><b>motorOn</b>: Actual: %d, Desired: %d <input type='button' id='button-motor-on' value='On'/><input type='button' id='button-motor-off' value='Off'/></li>\n"
                          "</ul>\n"
                          "<script>\n"
                          "[['button-motor-on', true], ['button-motor-off', false]].forEach(([elId, on]) => {\n"
                          "  const button = document.getElementById(elId);\n"
                          "  button.addEventListener('click', async _ => {\n"
                          "    try {\n"
                          "      const response = await fetch('/api/setProperty?controller=2&id=2&value=' + (on ? '1' : '0'), {method: 'POST'});\n"
                          "      console.log('setProperty complete', response);\n"
                          "    } catch(err) {\n"
                          "      console.error(`setProperty failed: ${err}`);\n"
                          "    }\n"
                          "  });\n"
                          "});\n"
                          "</script>\n"
                          "</body>"
                          "</html>",
                          properties[0].value[0],
                          properties[0].desiredValue[0]);
                  request->send(200, "text/html", response); });

    server.on("/api/getProperty", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  String controllerParam = request->getParam("controller")->value();
                  String idParam = request->getParam("id")->value();
                  uint8_t controllerId = strtoul(controllerParam.c_str(), NULL, 16);
                  uint16_t propertyId = strtoul(idParam.c_str(), NULL, 16);
                  IrrigationSystem::ControllerDefinition *definition = controllers.getControllerDefinition(controllerId);
                  uint32_t value = 0;
                  uint8_t *valueParts = (uint8_t *)&value; // assumes little endian (value is array of bytes with LSB first)
                  uint32_t desiredValue = 0;
                  uint8_t *desiredValueParts = (uint8_t *)&desiredValue; // assumes little endian (value is array of bytes with LSB first)
        int result = cih.getPropertyValue(
                      controllerId,
                      propertyId,
                      definition->getPropertyLength(propertyId),
                      definition->getPropertyReadOnly(propertyId),
                      valueParts,
                      desiredValueParts); // TODO send i2c request asynchronously
        if (result == 0)
        {
            request->send(200, "text/plain", String(value));
        }
        else
        {
            request->send(500, "text/plain", String(result));
        } });

    server.on("/api/setProperty", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  String controllerParam = request->getParam("controller")->value();
                  String idParam = request->getParam("id")->value();
                  String valueParam = request->getParam("value")->value();
                  uint8_t controllerId = strtoul(controllerParam.c_str(), NULL, 16);
                  uint16_t propertyId = strtoul(idParam.c_str(), NULL, 16);
                  uint32_t value = strtoul(valueParam.c_str(), NULL, 16);
                  uint8_t valueLength = controllers.getControllerDefinition(controllerId)->getPropertyLength(propertyId);
                  uint8_t *valueParts = (uint8_t *)&value;                                        // assumes little endian (value is array of bytes with LSB first)
        int result = cih.setPropertyDesiredValue(controllerId, propertyId, valueLength, valueParts); // TODO send i2c request asynchronously
        if (result == 0)
        {
            request->send(200, "text/plain", String(value));
        }
        else
        {
            request->send(500, "text/plain", String(result));
        } });

    server.onNotFound(onRequest);
    server.onRequestBody(onBody);
    server.begin();
}

void loop()
{
    cih.getPropertyValue(properties[0].controller, properties[0].id, properties[0].size, properties[0].readOnly, properties[0].value, properties[0].desiredValue);
    logProperties();
    delay(2000);
}
