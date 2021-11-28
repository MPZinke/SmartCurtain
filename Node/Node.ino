
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
#include <SPI.h>

#include "Curtain.h"
#include "Exceptions.h"
#include "Global.h"
#include "Gpio.h"
#include "Transmission.h"


void setup()
{
	// ———— GPIO SETUP ————
	pinMode(Configure::Hardware::CLOSE_PIN, INPUT);  // now analog, technically do not need
	pinMode(Configure::Hardware::OPEN_PIN, INPUT);  // now analog, technically do not need

	pinMode(Configure::Hardware::DIRECTION_PIN, OUTPUT);
	pinMode(Configure::Hardware::ENABLE_PIN, OUTPUT);
	pinMode(Configure::Hardware::PULSE_PIN, OUTPUT);

	Gpio::disable_motor();  // don't burn up the motor

	// ———— GLOBAL VARIABLES ————
#if __ETHERNET__
	// ethernet setup
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	Ethernet.begin(Configure::Network::MAC_ADDRESS, Configure::Network::NODE_HOST, Configure::Network::ROUTER_GATEWAY,
	  Configure::Network::SUBNET_MASK);  // connect to LAN
#elif __WIFI__
	// wifi setup
	WiFi.mode(WIFI_STA);
	esp_wifi_set_mac(WIFI_IF_STA, Configure::Network::MAC_ADDRESS);
	WiFi.begin(Configure::Network::SSID, Configure::Network::PASSWORD);
	while(WiFi.status() != WL_CONNECTED) delay(500);
#endif

	Global::server.begin();

//TODO: move completely closed if endstop
}


void loop()
{
	Gpio::disable_motor();  // don't burn up the motor

	StaticJsonDocument<Global::JSON_BUFFER_SIZE> json_document = decode_json();

	switch(json_document[Transmission::QUERY_TYPE_KEY])
	{
		case Transmission::Literals::JSON::Value::STATUS:
			Transmission::send_status();
			break;

		// Reset curtain by moving it from alleged current position to close to actual current position.
		case Transmission::Literals::JSON::Value::RESET:
			Gpio::move_until_closed();

		// Move to position
		case Transmission::Literals::JSON::Value::MOVE:
			Transmission::respond_with_json_and_stop(Transmission::Responses::VALID_RESPONSE);
			Curtain::Curtain curtain(json_document);  // setup data (things are getting real interesting...)
			curtain.move();

			// clean up and update curtain
			curtain.set_location();
			curtain.send_hub_serialized_info();
			break;

		default:
			throw HTTP_Exception(404, "Unknown query type");
	}

	end_catch:

	delay(700);
}


// RETURN: The JSON Document object.
// THROWS: HTTPS_Exceptions
StaticJsonDocument<Global::JSON_BUFFER_SIZE> decode_json()
{
	Global::client = Transmission::wait_for_request();

	// bad message: retry later
	char* json_buffer = Transmission::read_transmission_data_into_buffer();
	if(!json_buffer)
	{
		Exceptions::throw_HTTP_204("Could not read transmission into json_buffer");
	}

	// Decode JSON
	StaticJsonDocument<Global::JSON_BUFFER_SIZE> json_document;
	bool successfully_deserialized = !deserializeJson(json_document, json_buffer);
	delete[] json_buffer;

	if(!successfully_deserialized)
	{
		Exceptions::throw_HTTP_400("Could not decode json");
	}

	return json_document;
}
