/***************************************************************************************************
  (c) NewTec GmbH 2020   -   www.newtec.de
  $URL: https://github.com/NewTec-GmbH/esp32-can-iot $
***************************************************************************************************/
/**
@addtogroup Settings
@{
@file       WLAN.cpp

WiFi configuration of ESP32

* @}
***************************************************************************************************/
/* INCLUDES ***************************************************************************************/
#include "Settings.h"
#include "WLAN.h"
#include <WiFi.h>
#include "Board.h"

/* C-Interface ************************************************************************************/
extern "C"
{
}

/* CONSTANTS **************************************************************************************/
static const uint16_t WIFI_TIMEOUT_MS = 20000; /**< Maximum wait time to establish the WiFi connection */
static const String DIRECTORY = "WLAN";
static String STA_SSID = "";
static String STA_PASSWORD = "";
static String AP_SSID = "ESP32";
static String AP_PASSWORD = "hochschuleulm";

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/

/* PROTOTYPES *************************************************************************************/
/**
*   Determines the state of the WiFiModeSelect Button to enter AP Mode
*   @return bool AP Mode. If true, will request the AP mode. If false, requests STA Mode
*/
static void readWiFiMode();

/**
* Connects to the WiFi AP when requested
* @return true when succesfully connected. False otherwise.
*/
static bool connectWiFi();

/* VARIABLES **************************************************************************************/
IPAddress m_serverIP; /**< Stores the IP Address of the ESP32 */
bool m_APMode = true;

/* PUBLIC METHODES ********************************************************************************/
/**************************************************************************************************/
const String &wlan::getSTA_SSID()
{
    return STA_SSID;
}
/**************************************************************************************************/
const String &wlan::getSTA_PASS()
{
    return STA_PASSWORD;
}

/**************************************************************************************************/
const String &wlan::getAP_SSID()
{
    return AP_SSID;
}

/**************************************************************************************************/
const String &wlan::getAP_PASS()
{
    return AP_PASSWORD;
}

/**************************************************************************************************/
const bool &wlan::getAP_MODE()
{
    return m_APMode;
}

/**************************************************************************************************/
bool wlan::begin()
{
    bool success = true;
    Board::apLED.write(HIGH);
    Board::staLED.write(HIGH);

    Settings::get(DIRECTORY, "STA_SSID", STA_SSID, "");
    Settings::get(DIRECTORY, "STA_Password", STA_PASSWORD, "");

    readWiFiMode();
    if (m_APMode)
    {
        Board::staLED.write(LOW);
        if (!WiFi.softAP(AP_SSID.c_str(), AP_PASSWORD.c_str()))
        {
            success = false;
        }

        m_serverIP = WiFi.softAPIP();
    }
    else
    {
        Board::apLED.write(LOW);
        if (!WiFi.mode(WIFI_STA))
        {
            success = false;
        }
        else if (WiFi.begin(STA_SSID.c_str(), STA_PASSWORD.c_str()) == WL_CONNECT_FAILED)
        {
            success = false;
        }
        else if (!connectWiFi())
        {
            success = false;
        }

        m_serverIP = WiFi.localIP();
    }
    return success;
}

/**************************************************************************************************/
/**
*   Calls next request on DNS Server
*   return true if a restart is requested
*/
bool wlan::checkConnection()
{
    bool success = true;
    if (WiFi.getMode() == WIFI_STA && WiFi.status() != WL_CONNECTED)
    {
        if (!connectWiFi())
        {
            success = false;
        }
    }
    return success;
}

IPAddress wlan::getIPAddress()
{
    return m_serverIP;
}

void wlan::saveConfig(const String &key, const String &value)
{
    Settings::save(DIRECTORY, key, value);
}

/* PROTECTED METHODES *****************************************************************************/

/* PRIVATE METHODES *******************************************************************************/

/* EXTERNAL FUNCTIONS *****************************************************************************/

/* INTERNAL FUNCTIONS *****************************************************************************/
/**************************************************************************************************/
static void readWiFiMode()
{
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
            m_APMode = !currentBtnState;
        }

        previousBtnState = currentBtnState;
    }
}

/**************************************************************************************************/
bool connectWiFi()
{
    bool success = true;

    unsigned long startAttempTime = millis();

    while ((WiFi.status() != WL_CONNECTED) && ((millis() - startAttempTime) < WIFI_TIMEOUT_MS))
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

    return success;
}