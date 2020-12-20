
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Ethernet connection version of smart curtain. This is primarily desired for a Hub-Node model, where   *
*       The hub schedules events, and the node (this code) queries and activates those events. On event, curtain is    *
*       driven to location, where it updates the Hub. The primary communication is JSON through HTTP POST requests. If *
*       auto_correct option is set, the Hub will attempt to correct the curtain's location, assuming length,           *
*       direction, etc are correct. If the auto_calibrate flag is set and the curtain movement spans the entire rod,   *
*       the curtain will count the total steps taken (±1) and use that as the new length. Information is updated after *
*       each cycle by posting a JSON to the Hub.                                                                       *
*       **Main arduino file**                                                                                          *
*      CONVENTION: Position 0 is CLOSED                                                                                *
*   BUGS:   - If direction option is wrong, gears will be ground :)                                                    *
*           - If the curtain's length is > (4294967295 - allowable_difference), curtain may assume                     *
*             open position = close position.                                                                          *
*   FUTURE: - Add cool temperature, light, & thermostat data.                                                          *
*           - Figure out a solution for needing Hub to specify length of curtain.                                      *
*                                                                                                                      *
***********************************************************************************************************************/


#include <assert.h>
#include <Ethernet.h>
#include <SPI.h>

#include "Curtain.h"
#include "Global.h"
#include "GPIO.h"
#include "Transmission.h"
#include "User.h"


#define CURTAIN_VAR "curtain="
#define TYPE_VAR_EVENT "&type=event"
#define TYPE_VAR_DONE "&type=done"
#define STATE_VAR "&state="


void setup()
{
	// ———— GPIO SETUP ————
	pinMode(GPIO::CLOSE_PIN, INPUT);  // now analog, technically do not need
	pinMode(GPIO::OPEN_PIN, INPUT);  // now analog, technically do not need
	pinMode(GPIO::DIRECTION_PIN, OUTPUT);
	pinMode(GPIO::ENABLE_PIN, OUTPUT);
	pinMode(GPIO::PULSE_PIN, OUTPUT);

	GPIO::disable_motor();  // don't burn up the motor

	// ———— GLOBAL VARIABLES ————
	// ethernet setup
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	while(!Ethernet.begin((uint8_t*)User::curtain_mac)) delay(1000);  // wait while not connected to LAN
	while(!Global::client.connect(Global::server, 80)) delay(1000);  // wait while not connected to device

	delay(User::loop_wait);
}


void loop()
{
	GPIO::disable_motor();  // don't burn up the motor

	byte packet_buffer[Transmission::BUFFER_LENGTH];
	Transmission::post_data(String(CURTAIN_VAR)+User::curtain_number+TYPE_VAR_EVENT+STATE_VAR+GPIO::state());
	if(Transmission::read_state_response_successfully_into_buffer(packet_buffer))
	{
		Curtain::Curtain curtain(packet_buffer);  // setup data (things are getting real interesting...)

		if(!curtain.event_moves_to_an_end()) GPIO::move(curtain);
		// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
		// ing or closing curtain.
		// Also does not take into account if desire == current.  It can be 'move 0' or ignored by Master.
		else
		{
			if(curtain.should_calibrate_across()) curtain.length(GPIO::calibrate_to_opposite(curtain.direction()));
			else if(curtain.state_of_desired_position() == Curtain::OPEN) GPIO::move_until_open(curtain.direction());
			else GPIO::move_until_closed(curtain.direction());
		}

		// clean up and update curtain
		curtain.set_location();
		curtain.encode(packet_buffer);
		Transmission::post_data(String(CURTAIN_VAR)+User::curtain_number+TYPE_VAR_DONE+STATE_VAR+(char*)packet_buffer);
	}

	Global::client.stop();
}
