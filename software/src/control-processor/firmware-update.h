#include <HTTPClient.h>
#include <Update.h>
#include "error-handler.h"

void updateFirmware(char *url, const ErrorHandler &errorHandler)
{
    errorHandler.handleError(ErrorComponent::Update, 1, "Update requested");
    HTTPClient http;
    http.begin(url);
    int responseCode = http.GET();
    if (responseCode < 200 || responseCode >= 300)
    {
        errorHandler.handleError(ErrorComponent::Config, responseCode, "Download failed");
        return;
    }
    int responseLength = http.getSize();
    if (responseLength <= 0)
    {
        errorHandler.handleError(ErrorComponent::Update, 2, "Response size missing");
        return;
    }
    uint8_t buffer[128] = {0};
    // Start firmware update process
    errorHandler.handleError(ErrorComponent::Update, responseLength, "Starting update");
    Update.begin(UPDATE_SIZE_UNKNOWN);
    // Get TCP stream
    WiFiClient *stream = http.getStreamPtr();

    int remainingLen = responseLength;
    while (http.connected() && remainingLen > 0)
    {
        int size = stream->available();
        if (size)
        {
            int bytesRead = stream->readBytes(buffer, ((size > sizeof(buffer)) ? sizeof(buffer) : size));
            Update.write(buffer, bytesRead);
            remainingLen -= bytesRead;

            // // pass to function
            // writeFirmware(buffer, bytesRead);
            // if (remainingLen > 0)
            // {
            //     remainingLen -= bytesRead;
            // }
        }
        delay(1);
    }
    Update.end(true);
    if (remainingLen == 0)
    {
        ESP.restart();
    }
    else
    {
        errorHandler.handleError(ErrorComponent::Update, 4, "Connection closed before update completed");
    }
}

// Function to update firmware incrementally
// Buffer is declared to be 128 so chunks of 128 bytes
// from firmware is written to device until server closes
// void writeFirmware(uint8_t *data, size_t len)
// {
//     Update.write(data, len);
//     currentLength += len;
//     // Print dots while waiting for update to finish
//     Serial.print('.');
//     // if current length of written firmware is not equal to total firmware size, repeat
//     if (currentLength != totalLength)
//         return;
//     Update.end(true);
//     Serial.printf("\nUpdate Success, Total Size: %u\nRebooting...\n", currentLength);
//     // Restart ESP32 to see changes
//     ESP.restart();
// }
