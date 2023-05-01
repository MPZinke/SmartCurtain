
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
	
	// ———— WIFI ———— //
	{
		using namespace Config::Network;
		WiFi.mode(WIFI_STA);
		esp_wifi_set_mac(WIFI_IF_STA, MAC_ADDRESS);
		WiFi.begin(SSID, PASSWORD);
		while(WiFi.status() != WL_CONNECTED)
		{
			delay(500);
		}
	}

	// ———— MQTT ———— //
	{
		while(!Global::mqtt_client.connect(Config::Network::BROKER_DOMAIN, Config::Network::BROKER_PORT))
		{
			delay(500);
		}

		using namespace Message::Literal::MQTT;
		Global::mqtt_client.onMessage(Control::process_message);
		Global::mqtt_client.subscribe(ALL_CURTAINS);
		Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+MOVE_SUFFIX);
		Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+STATUS_SUFFIX);
		Global::mqtt_client.subscribe(String(CURTAIN_PATH_PREFIX)+Config::Curtain::CURTAIN_ID+UPDATE_SUFFIX);
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
