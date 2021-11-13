#include <Wire.h>
#include <WiFi.h>

#define I2C_SLAVE_ADDRESS 10

const char *ssid = "*";
const char *password = "*";
WiFiServer server(80);

void setup()
{
    pinMode(21, INPUT_PULLUP);
    pinMode(22, INPUT_PULLUP);
    Serial.begin(9600);
    Wire.begin();

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop()
{
    WiFiClient client = server.available();
    if (client)
    {
        Serial.println("[log] New client");
        unsigned long startTime = millis();
        int lastByte = 0;
        bool received = false;
        bool son = false;
        bool soff = false;
        bool bat = false;
        char lineBuffer[32] = {0};
        char *p = lineBuffer;
        // Read request head and echo to Serial
        while (client.connected() && millis() < startTime + 2000)
        {
            if (client.available())
            {
                int b = client.read();
                Serial.write(b);
                if (b != '\r' && b != '\n' && p < lineBuffer + 32)
                {
                    *p = b;
                    ++p;
                    if (!son && !soff && !bat)
                    {
                        if (p - lineBuffer >= 7 && strncmp(lineBuffer, "GET /on", 7) == 0)
                        {
                            son = true;
                        }
                        else if (p - lineBuffer >= 8 && strncmp(lineBuffer, "GET /off", 8) == 0)
                        {
                            soff = true;
                        }
                        else if (p - lineBuffer >= 8 && strncmp(lineBuffer, "GET /bat", 8) == 0)
                        {
                            bat = true;
                        }
                    }
                }
                else
                {
                    p = lineBuffer;
                }
                // Check for end of request
                if (b != '\r')
                {
                    if (b == '\n' && lastByte == '\n')
                    {
                        received = true;
                        break;
                    }
                    lastByte = b;
                }
            }
        }
        if (received)
        {
            bool i2cRequest = son || soff || bat;
            if (i2cRequest)
            {
                Wire.beginTransmission(I2C_SLAVE_ADDRESS);
                if (son)
                {
                    Wire.print("SOLON\n");
                    Serial.println("[log] Requested SOLON");
                }
                else if (soff)
                {
                    Wire.print("SOLOFF\n");
                    Serial.println("[log] Requested SOLOFF");
                }
                else if (bat)
                {
                    Wire.print("BAT\n");
                    Serial.println("[log] Requested BAT");
                }
                uint8_t error = Wire.endTransmission();
                if (error != 0)
                {
                    Serial.print("[log] I2C transmission error: ");
                    Serial.print(error);
                    Serial.println(".");
                }
                delay(5000);
                Serial.println("[log] Sending redirect response");
                client.println("HTTP/1.1 303 See Other");
                client.println("Location: /");
                client.println("Connection: close");
                client.println();
            }
            else
            {
                Serial.println("[log] Sending webpage response");
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println();
                client.println("<!DOCTYPE html><html>");
                client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                client.println("<link rel=\"icon\" href=\"data:,\">");
                client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                client.println(".button2 {background-color: #555555;}</style>");
                client.println("<title>Irrigation Controller</title></head>");
                client.println("<body><h1>Irrigation Controller</h1>");

                client.println("<p><a href=\"/on\"><button class=\"button\">On</button></a></p>");
                client.println("<p><a href=\"/off\"><button class=\"button\">Off</button></a></p>");
                client.println("<p><a href=\"/bat\"><button class=\"button\">Check battery</button></a></p>");
                client.print("<p>Result: ");
                Serial.print("[log] I2C response: ");
                Wire.requestFrom(I2C_SLAVE_ADDRESS, 32);
                while (Wire.available())
                {
                    int b = Wire.read();
                    if (b >= 32 && b < 127)
                    {
                        Serial.write(b);
                        client.write(b);
                    }
                }
                Serial.println();
                client.println("</p>");
                client.println("</body></html>");
                client.println();
            }
        }
        else
        {
            Serial.println("[log] Didn't receive full request");
        }
    }

    // if (Serial.available())
    // {
    //     // Send transmission of up to 32 bytes
    //     Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    //     for (int i = 0; i < 32 && Serial.available(); ++i)
    //     {
    //         int b = Serial.read();
    //         Wire.write(b);
    //     }
    //     uint8_t error = Wire.endTransmission();
    //     if (error != 0)
    //     {
    //         Serial.print("Transmission error: ");
    //         Serial.print(error);
    //         Serial.println(".");
    //     }

    //     delay(3000);
    //     // Receive response up to 32 bytes
    //     Wire.requestFrom(I2C_SLAVE_ADDRESS, 32);
    //     while (Wire.available())
    //     {
    //         int b = Wire.read();
    //         if (b > 0)
    //         {
    //             Serial.write(b);
    //         }
    //     }
    // }
    // delay(100);
}
