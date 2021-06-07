
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
#include <assert.h>
#include <WiFi.h>
#include <SPI.h>

#include "Curtain.h"
#include "Global.h"
#include "Gpio.h"
#include "Transmission.h"
#include "User.h"


void setup()
{
	// ———— GPIO SETUP ————
	// pinMode(Gpio::CLOSE_PIN, INPUT);  // now analog, technically do not need
	// pinMode(Gpio::OPEN_PIN, INPUT);  // now analog, technically do not need
	// pinMode(Gpio::DIRECTION_PIN, OUTPUT);
	// pinMode(Gpio::ENABLE_PIN, OUTPUT);
	// pinMode(Gpio::PULSE_PIN, OUTPUT);

	// Gpio::disable_motor();  // don't burn up the motor

	// ———— GLOBAL VARIABLES ————
#ifdef __ETHERNET__
	// ethernet setup
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	Ethernet.begin(User::mac_address, User::node_host, User::router_gateway, User::subnet_mask);  // connect to LAN

#elif __WIFI__
	// wifi setup
	WiFi.begin(User::SSID, User::password);
	while(WiFi.status() != WL_CONNECTED) delay(500);
#endif

	Global::server.begin();
}


void loop()
{
	// Gpio::disable_motor();  // don't burn up the motor

	WiFiClient client = Transmission::wait_for_request();
	Global::client = Transmission::wait_for_request();
	// while(Global::client->available()) Serial.print((char)Global::client->read());  //TESTING

	// bad message: retry later
	char* json_buffer = Transmission::read_transmission_data_into_buffer();
	if(!json_buffer) return;

	StaticJsonDocument<Transmission::BUFFER_LENGTH> json_document;
	if(deserializeJson(json_document, json_buffer)) return delete json_buffer;
	delete json_buffer;

	Curtain::Curtain curtain(json_document);  // setup data (things are getting real interesting...)
	Transmission::send_OK_response_and_stop_client();

	// 	if(!curtain.event_moves_to_an_end()) Gpio::move(curtain);
	// 	// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
	// 	// ing or closing curtain.
	// 	// Also does not take into account if desire == current.  It can be 'move 0' or ignored by Master.
	// 	else
	// 	{
	// 		if(curtain.should_calibrate_across()) curtain.length(Gpio::calibrate_to_opposite(curtain.direction()));
	// 		else if(curtain.state_of_desired_position() == Curtain::OPEN) Gpio::move_until_open(curtain.direction());
	// 		else Gpio::move_until_closed(curtain.direction());
	// 	}

	// clean up and update curtain
	curtain.set_location();
	curtain.send_hub_serialized_info();

	delay(700);
}
