
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Ethernet connection version of smart curtain. This is primarily desired for a Hub-Node model, where   *
*       The Hub schedules events, and the node (this code) queries and activates those events. On event, curtain is    *
*       driven to location, where it updates the Hub. The primary communication is JSON through HTTP POST requests. If *
*       auto_correct option is set, the Hub will attempt to correct the curtain's location, assuming length,           *
*       direction, etc are correct. If the auto_calibrate flag is set and the curtain movement spans the entire rod,   *
*       the curtain will count the total steps taken (±1) and use that as the new length. Information is updated after *
*       each cycle by posting a JSON to the Hub.                                                                       *
*       **Main arduino file**                                                                                          *
*      CONVENTION: Position 0 is CLOSED                                                                                *
*   BUGS:   - If direction option is wrong, gears will be ground :)                                                    *
*           - If the curtain's (driver) step length is > (4294967295 - allowable_difference), curtain may assume       *
*             open position = close position.                                                                          *
*   FUTURE: - Add cool temperature, light, & thermostat data.                                                          *
*           - Figure out a solution for needing Hub to specify length of curtain.                                      *
*                                                                                                                      *
***********************************************************************************************************************/


#include <ArduinoJson.h>
#include <EEPROM.h>
#include <esp_wifi.h>
#include <soc/rtc_wdt.h>
#include <SPI.h>
#include <WiFi.h>


#include "Headers/Config.hpp"
#include "Headers/Global.hpp"

#include "Headers/Control.hpp"
#include "Headers/Curtain.hpp"
#include "Headers/Event.hpp"
#include "Headers/Exception.hpp"
#include "Headers/Hardware.hpp"
#include "Headers/Message.hpp"
#include "Headers/Movement.hpp"


void setup_wifi_credentials()
{
	char* SSID = (char*)Config::Network::WiFi::SSID;
	for(uint8_t x = 0, character = EEPROM.read(x); x < 32 && character != '\0'; x++, character = EEPROM.read(x))
	{
		SSID[x] = character;
		SSID[x+1] = '\0';
	}

	char* PASSWORD = (char*)Config::Network::WiFi::PASSWORD;
	for(uint8_t x = 32, character = EEPROM.read(x); x < 64 && character != '\0'; x++, character = EEPROM.read(x))
	{
		PASSWORD[x] = character;
		PASSWORD[x+1] = '\0';
	}
}


bool setup_wifi()
{
	setup_wifi_credentials();
	if(Config::Network::WiFi::SSID[0] == '\0')
	{
		return false;
	}

	WiFi.mode(WIFI_STA);
	esp_wifi_set_mac(WIFI_IF_STA, Config::Network::MAC_ADDRESS);

	WiFi.begin(Config::Network::WiFi::SSID, Config::Network::WiFi::PASSWORD);
	for(uint8_t x = 0; x < 30; x++)
	{
		if(WiFi.status() == WL_CONNECTED)
		{
			return true;
		}

		delay(500);
	}

	return false;
}


void setup_mqtt_credentials()
{

}


bool setup_mqtt()
{
	for(uint8_t x = 0; x < 30; x++)
	{
		if(Global::mqtt_client.connect(Config::Network::MQTT::BROKER_DOMAIN, Config::Network::MQTT::PORT))
		{
			using namespace Message::Literal::MQTT;
			Global::mqtt_client.onMessage(Control::process_message);
			Global::mqtt_client.subscribe(ALL_CURTAINS_MOVE);
			Global::mqtt_client.subscribe(ALL_CURTAINS_STATUS);
			Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+MOVE_SUFFIX);
			Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+STATUS_SUFFIX);
			Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+UPDATE_SUFFIX);

			return true;
		}

		delay(500);
	}

	return false;
}


void setup()
{
	// ———— GPIO SETUP ———— //
	{
		using namespace Config::Hardware;
		pinMode(CLOSE_PIN, INPUT);
	
		pinMode(DIRECTION_PIN, OUTPUT);
		pinMode(ENABLE_PIN, OUTPUT);
		pinMode(PULSE_PIN, OUTPUT);
	
		Hardware::disable_motor();  // don't burn up the motor
	}

	// ———— NETWORK SETUP ———— //
	if(!setup_wifi() || !setup_mqtt())
	{

	}
	else
	{
		// Server
	}

	// ———— Threads ———— //
	{
		// function, name, stack size, send the bytes as a parameter, priority, task handler, core (0, 1)
		xTaskCreatePinnedToCore((TaskFunction_t)Control::loop, "MQTT", 10000, NULL, 2, NULL, 0);
		// Reset Curtain
		Global::curtain.is_moving(true);
		xTaskCreatePinnedToCore((TaskFunction_t)Movement::reset, "Resetting", 10000, NULL, 2, NULL, 1);

		// Prevent infinite loop detection
		rtc_wdt_protect_off();
		rtc_wdt_disable();
		disableCore0WDT();
		disableLoopWDT();
	}
}


void loop()
{}
