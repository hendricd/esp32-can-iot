/***************************************************************************************************
  (c) NewTec GmbH 2020   -   www.newtec.de
  $URL: https://github.com/NewTec-GmbH/esp32-can-iot $
***************************************************************************************************/
/**
@addtogroup Webserver
@{
@file       ESPServer.cpp

Handler for ESP32 WebServer. @ref ESPServer.h

* @}
***************************************************************************************************/
/* INCLUDES ***************************************************************************************/
#include <ESP_Server.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <Board.h>
#include <Web_config.h>
#include <Pages.h>
#include <CaptivePortal.h>

/* C-Interface ************************************************************************************/
extern "C"
{
}

/* CONSTANTS **************************************************************************************/

static AsyncWebServer server(WebConfig::WEBSERVER_PORT); /**< Instance of AsyncWebServer*/
static DNSServer dnsServer;                              /**< Instance of DNS Server*/

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/

/* PROTOTYPES *************************************************************************************/
/**
*   Determines the state of the WiFiModeSelect Button to enter AP Mode
*   @return bool AP Mode. If true, will request the AP mode. If false, requests STA Mode
*/
static bool setAPMode();
static bool restartRequested = false;

/* VARIABLES **************************************************************************************/
IPAddress m_serverIP;       /**< Stores the IP Address of the ESP32 */

/* PUBLIC METHODES ********************************************************************************/

/**************************************************************************************************/
/**
* Registers the handlers on the server, depending on the WiFi Mode chosen
* @param bool apModeRequested  determines if AP Mode or STA Mode are requested, to choose the correct handlers
* return success
*/
bool ESPServer::init(bool apModeRequested)
{
    bool success = true;

    if (apModeRequested)
    {
        CaptivePortal::init(server);
    }
    else
    {
        Pages::init(server);
    }

    return success;
}

/**************************************************************************************************/

/**
*   Initializing of AsyncWebServer, DNS and WiFi capabilities. 
*   return success
*/
bool ESPServer::begin()
{
    bool success = true;
    WebConfig::importConfig(); /*< Imports Credentials from Flash Memory */

    if (setAPMode())
    {
        WiFi.softAP(WebConfig::getAP_SSID().c_str(), WebConfig::getAP_PASS().c_str());
        m_serverIP = WiFi.softAPIP();

        ESPServer::init(true);
    }
    else
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WebConfig::getSTA_SSID().c_str(), WebConfig::getSTA_PASS().c_str());

        unsigned long startAttempTime = millis();

        while (WiFi.status() != WL_CONNECTED && (millis() - startAttempTime) < WebConfig::WIFI_TIMEOUT_MS)
        {
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            success = false;
        }
        else
        {
            m_serverIP = WiFi.localIP();
        }

        ESPServer::init(false);
    }

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);

    if (!dnsServer.start(WebConfig::DNS_PORT, "*", m_serverIP))
    {
        success = false;
    }
    else if (!SPIFFS.begin())
    {
        success = false;
    }

    server.begin();
    return success;
}

/**************************************************************************************************/

/**
*   Disconnects and disables the WebServer
*/
bool ESPServer::end()
{
    server.end();
    SPIFFS.end();
    dnsServer.stop();
    return WiFi.disconnect(true, true);
}

/**************************************************************************************************/
/**
*   Calls next request on DNS Server
*   return true
*/
bool ESPServer::handleNextRequest()
{
    dnsServer.processNextRequest();
    restartRequested = CaptivePortal::isRestartRequested();
    return restartRequested;
}

AsyncWebServer &ESPServer::getInstance()
{
    return server;
}

/* PROTECTED METHODES *****************************************************************************/

/* PRIVATE METHODES *******************************************************************************/

/* EXTERNAL FUNCTIONS *****************************************************************************/

/* INTERNAL FUNCTIONS *****************************************************************************/
/**************************************************************************************************/
static bool setAPMode()
{

    bool apMode = false;

    uint8_t currentBtnState = LOW;
    uint8_t previousBtnState = LOW;
    uint32_t lastDebounceTime = 0;

    const uint8_t DEBOUNCE_DELAY = 50;
    const uint32_t SETUP_TIME = 2000;
    const uint32_t START_TIME = millis();

    while ((millis() - START_TIME) < SETUP_TIME)
    {
        currentBtnState = Board::wifiModeSelect.read();

        if (currentBtnState != previousBtnState)
        {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
        {
            apMode = currentBtnState;
        }

        previousBtnState = currentBtnState;
    }

    return apMode;
}