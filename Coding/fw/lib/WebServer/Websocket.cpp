/***************************************************************************************************
  (c) NewTec GmbH 2020   -   www.newtec.de
  $URL: https://github.com/NewTec-GmbH/esp32-can-iot $
***************************************************************************************************/
/**
@addtogroup Webserver
@{
@file       Websocket.cpp

Configuration of ESP32 WebSocket. @ref Websocket.h

* @}
***************************************************************************************************/
/* INCLUDES ***************************************************************************************/
#include "Websocket.h"
#include <SPIFFS.h>

/* C-Interface ************************************************************************************/
extern "C"
{
}

/* CONSTANTS **************************************************************************************/
AsyncWebSocket ws("/ws");

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/

/* PROTOTYPES *************************************************************************************/
static String processor(const String &var);
static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                    void *arg, uint8_t *data, size_t len);

/* VARIABLES **************************************************************************************/
static String inputBuffer;

/* PUBLIC METHODES ********************************************************************************/

/**************************************************************************************************/

/*
*   Initializes the WebSocket Service
*/
void websocket::init(AsyncWebServer &server)
{
    server.on("/communication", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/ws.html", String(), false, processor);
    });

    ws.onEvent(onEvent);
    server.addHandler(&ws);
    inputBuffer = "";
}

/**************************************************************************************************/

/*
*   Send WebSocket Message
*/
void websocket::send(String message)
{
    String systime = String(millis());
    message += systime.substring(0, 7);
    ws.textAll(message);
}

/**************************************************************************************************/

/*
*   Receive WebSocket Message
*/
bool websocket::receive(char &c)
{
    bool available = false;
    if (inputBuffer.length() != 0)
    {
        available = true;
        c = inputBuffer[0];
        inputBuffer.remove(0, 1);
    }

    return available;
}

/* PROTECTED METHODES *****************************************************************************/

/* PRIVATE METHODES *******************************************************************************/

/* EXTERNAL FUNCTIONS *****************************************************************************/

/* INTERNAL FUNCTIONS *****************************************************************************/

/**************************************************************************************************/

/*
*   Page Processor
*/
static String processor(const String &var)
{
    if (var == "STATE")
    {
    }
    return "";
}

/**************************************************************************************************/

/*
*   Handler for incoming Web Socket Message
*/
static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    String temp;
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        for (int i = 0; i < len; i++)
        {
            inputBuffer += (char)data[i];
        }
    }
}

/**************************************************************************************************/

/*
*   Handler for Web Socket Event
*/
static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                    void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}