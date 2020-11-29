
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Ethernet connection version of smart curtain.  Queries server and waits                               *
*		on response for next event.  On event, drives curtain to location.                                             *
*		**Main arduino file**                                                                                          *
*      CONVENTION: Position 0 is CLOSED                                                                                *
*   BUGS:                                                                                                              *
*   FUTURE: - Add cool temperature, light, & thermostat data.                                                          *
*                                                                                                                      *
***********************************************************************************************************************/


#include <SPI.h>
#include <Ethernet.h>

#include "Curtain.h"
#include "Global.h"
#include "GPIO.h"
#include "Transmission.h"
#include "User.h"


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

	byte packet_buffer[Transmission::PACKET_LENGTH];
	Transmission::post_data(String("curtain=") + User::curtain_number);
	if(Transmission::read_state_response_successfully_into_buffer(packet_buffer))
	{
		Curtain::Curtain curtain(packet_buffer);  // setup data (things are getting real interesting...)

		// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
		// ing or closing curtain.
		// Also does not take into account if current == current.  It can be 'move 0' or ignored by Master.
		if(curtain.event_moves_to_an_end())
		{
			if(curtain.should_calibrate_across()) curtain.length(GPIO::calibrate_to_opposite(curtain.direction()));
			else if(curtain.state_of_desired_position() == Curtain::OPEN) GPIO::move_until_open(curtain.direction());
			else GPIO::move_until_closed(curtain.direction());
		}
		else if(!GPIO::move(curtain, true))
		{
			// failed to move correctly: set location to GPIO::state()
		}
		curtain.set_location();
	}
	else
	{

	}
	Global::client.stop();
}
