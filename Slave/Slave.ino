
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

#include "Global.h"
#include "Transmissions.h"
#include "User.h"


void setup()
{
	// ———— GPIO SETUP ————
	pinMode(CLOSE_PIN, INPUT);
	pinMode(OPEN_PIN, INPUT);
	pinMode(DIRECTION_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);
	pinMode(PULSE_PIN, OUTPUT);

	disable_motor();  // don't burn up the motor

	// ———— GLOBAL VARIABLES ————
	// ethernet setup
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	while(!Ethernet.begin(User::curtain_mac)) delay(1000);  // wait while not connected to LAN
	while(!Global::client.connect(Global::server, 80)) delay(1000);  // wait while not connected to device

	delay(LOOP_WAIT);
}


void loop()
{
	btye packet_buffer[Transmission::PACKET_LENGTH];

	Transmission::post_data("curtain=" User::curtain_number);
	if(Transmission::read_state_response_successfully_into_buffer(packet_buffer))
	{
		Curtain::Curtain curtain(packet_buffer);  // setup data (things are getting real interesting...)

		// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
		// ing or closing curtain.
		// Also does not take into account if current == current.  It can be 'move 0' or ignored by Master.
		if(event_moves_to_an_end(curtain))
		{
			if(curtain.should_calibrate_across()) curtain.length(GPIO::calibrate_to_opposite(curtain.direction()));
			else if(curtain.state_of_desired_position() == OPEN) GPIO::move_until_open(curtain.direction);
			else GPIO::move_until_closed(curtain.direction);
		}
		else if(curtain.correct && !GPIO::move(curtain))
		{

		}
		else
		{

		}
		curtain.set_location();
	}

}
