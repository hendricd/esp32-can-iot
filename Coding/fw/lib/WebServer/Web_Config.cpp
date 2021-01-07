/***************************************************************************************************
  (c) NewTec GmbH 2020   -   www.newtec.de
  $URL: https://github.com/NewTec-GmbH/esp32-can-iot $
***************************************************************************************************/
/**
@addtogroup Webserver
@{
@file       Web_Config.cpp

Stores the Credentials and configuration of the Webserver. @ref Web_config.h

* @}
***************************************************************************************************/
/* INCLUDES ***************************************************************************************/
#include "Web_Config.h"

/* C-Interface ************************************************************************************/
extern "C"
{
}

/* CONSTANTS **************************************************************************************/
static const String DIRECTORY = "Server";
static String WEB_USER = "admin";
static String WEB_PASSWORD = "admin";

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/

/* PROTOTYPES *************************************************************************************/

/* VARIABLES **************************************************************************************/

/* PUBLIC METHODES ********************************************************************************/

/**************************************************************************************************/

/*
*   Returns Username for Authentification of Webpages
*/
const String &WebConfig::getWEB_USER()
{
    return WEB_USER;
}

/**************************************************************************************************/

/*
*   Returns Password for Authentification of Webpages
*/
const String &WebConfig::getWEB_PASS()
{
    return WEB_PASSWORD;
}

/* PROTECTED METHODES *****************************************************************************/

/* PRIVATE METHODES *******************************************************************************/

/* EXTERNAL FUNCTIONS *****************************************************************************/

/* INTERNAL FUNCTIONS *****************************************************************************/
