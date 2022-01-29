
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


#include <esp_wifi.h>
#include <SPI.h>


#include "Headers/Config.hpp"
#include "Headers/Global.hpp"

#include "Headers/Automation.hpp"
#include "Headers/Encoder.hpp"


TaskHandle_t automation_task;
TaskHandle_t encoder_task;


void setup()
{
	// ———— GPIO SETUP ————
	pinMode(Config::Hardware::CLOSE_PIN, INPUT);  // now analog, technically do not need
	pinMode(Config::Hardware::OPEN_PIN, INPUT);  // now analog, technically do not need

	pinMode(Config::Hardware::DIRECTION_PIN, OUTPUT);
	pinMode(Config::Hardware::ENABLE_PIN, OUTPUT);
	pinMode(Config::Hardware::PULSE_PIN, OUTPUT);

	Movement::disable_motor();  // don't burn up the motor

	// ———— GLOBAL VARIABLES ————
	// wifi setup
	WiFi.mode(WIFI_STA);
	esp_wifi_set_mac(WIFI_IF_STA, Config::Network::MAC_ADDRESS);
	WiFi.begin(Config::Network::SSID, Config::Network::PASSWORD);
	while(WiFi.status() != WL_CONNECTED) delay(500);

	Global::server.begin();

//TODO: move completely closed if endstop
	// Task Params: Function to implement the task, Name of the task, Stack size in words, Task input parame, 
	//  Priority of the task, Task handle to keep track of created task, Core where the task should run
	xTaskCreatePinnedToCore(Automation::automation_loop, "Automation", 10000, NULL, 0, &automation_task, 0);
	if(Config::Hardware::ENCODER)
	{
		xTaskCreatePinnedToCore(Encoder::encoder_loop, "Encoder", 10000, NULL, 1, &encoder_task, 1);
	}
}


void loop()
{}

