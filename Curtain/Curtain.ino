
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
#include "Headers/MQTT.hpp"
#include "Headers/Movement.hpp"
#include "Headers/StaticString.hpp"


void setup_GPIO()
{
	pinMode(Config::Hardware::CLOSE_PIN, INPUT);

	pinMode(Config::Hardware::DIRECTION_PIN, OUTPUT);
	pinMode(Config::Hardware::ENABLE_PIN, OUTPUT);
	pinMode(Config::Hardware::PULSE_PIN, OUTPUT);

	Hardware::disable_motor();  // Don't burn up the motor
}


void setup_WiFi()
{
	WiFi.mode(WIFI_STA);
	esp_wifi_set_mac(WIFI_IF_STA, Config::Network::Device::MAC_ADDRESS);

	WiFi.begin(Config::Network::WiFi::SSID, Config::Network::WiFi::PASSWORD);
	while(WiFi.status() == WL_CONNECTED)
	{
		delay(500);
	}
}


void setup_MQTT()
{
	Global::mqtt_client.setUsernamePassword(Config::Network::MQTT::USERNAME, Config::Network::MQTT::PASSWORD);
	while(!Global::mqtt_client.connect(Config::Network::MQTT::BROKER_DOMAIN, Config::Network::MQTT::PORT))
	{
		delay(500);
	}

	using namespace MQTT;
	Global::mqtt_client.onMessage(Control::process_message);
	Global::mqtt_client.subscribe(Topics::Literals::HOME_MOVE);

	Global::mqtt_client.subscribe((const char*)Topics::ROOM_MOVE);
	Global::mqtt_client.subscribe((const char*)Topics::CURTAIN_MOVE);
	Global::mqtt_client.subscribe((const char*)Topics::CURTAIN_UPDATE);
}


void setup_threading()
{
	// function, name, stack size, send the bytes as a parameter, priority, task handler, core (0, 1)
	xTaskCreatePinnedToCore((TaskFunction_t)Control::main, "MQTT", 10000, NULL, 2, NULL, 0);

	// Prevent infinite loop detection
	rtc_wdt_protect_off();
	rtc_wdt_disable();
	disableCore0WDT();
	disableLoopWDT();

	// Reset Curtain
	Global::curtain.is_moving(true);
	xTaskCreatePinnedToCore((TaskFunction_t)Movement::reset, "Resetting", 10000, NULL, 2, NULL, 1);
}


void setup()
{
	Serial.begin(9600);
	setup_GPIO();

	setup_WiFi();
	setup_MQTT();

	setup_threading();
	// Serial.println("Setup complete");
}


void loop()
{}
