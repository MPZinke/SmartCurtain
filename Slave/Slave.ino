
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

// #include <SPI.h>
// #include <Ethernet.h>

#include "Global.h"
#include "Transmissions.h"


IPAddress SERVER(MASTER_HOST_ARRAY);
EthernetClient CLIENT;  // the magician

btye PACKET_BUFFER[PACKET_LENGTH+1];


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
	PACKET_BUFFER[PACKET_LENGTH] = 0;  // make sure buffer does not ever read out of bound
	// ethernet setup
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	while(!Ethernet.begin(CURTAIN_MAC)) delay(1000);  // wait while not connected to LAN
	while(!CLIENT.connect(SERVER, 80)) delay(1000);  // wait while not connected to device

	delay(LOOP_WAIT);
}


void loop()
{
	Transmission::post_data("curtain=" DEF(CURTAIN_NUMBER));
	if(Transmission::read_state_response_successfully_into_buffer())
	{
		// setup data (things are getting real interesting...)
		Curtain curtain = Transmission::decode_response();

		// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
		// ing or closing curtain.
		// Also does not take into account if current == current.  It can be 'move 0' or ignored by Master.
		if(event_moves_to_an_end(curtain))
		{
			if(curtain.calibrate && moves_full_span(curtain))
				curtain.length = GPIO::calibrate_moving_to_opposite(curtain.direction);
			else if(state_of(curtain.desired_position, curtain.length) == OPEN)
				GPIO::move_until_open(curtain.direction);
			else GPIO::move_until_closed(curtain.direction);
		}
		else if(curtain.correct)
		{

		}
	}

}


// ————————————————————————————————————————————————————— POSITIONS —————————————————————————————————————————————————————

bool is_approximate_position(uint32_t position1, uint32_t position2)
{
	return	position1 - Global::WIGGLE_ROOM <= position2 
			  && position2 <= position1 + Global::WIGGLE_ROOM; 
}


bool is_approximate_position(uint32_t position1, uint32_t position2, uint32_t buffer_room)
{
	return position1 - buffer_room <= position2 && position2 <= position1 + buffer_room; 
}


bool moves_full_span(Curtain& curtain)
{
	CurtainState curtian_state = GPIO::state();
	CurtainState desired_state = state_of(curtain.desired_position, curtain.length);
	return	state == CLOSED && desired_state == OPEN || state == OPEN && desired_state == CLOSED;
}


void round_desired_position_if_close_to_end(Curtain& curtain)
{
	// set if closed
	if(is_approximate_position(curtain.desired_position, 0))
		curtain.desired_position = 0;
	// set if open
	if(is_approximate_position(curtain.desired_position, curtain.length))
		curtain.desired_position = curtain.length;
}


CurtainState state_of(uint32_t position, uint32_t curtain_length)
{
	if(is_approximate_position(position, 0)) return CLOSED;
	if(is_approximate_position(position, curtain_length)) return OPEN;
	return MIDDLE;
}


// void set_current_position_if_does_not_match_sensors(Curtain& curtain)
// {
// 	if(GPIO::is_closed() && !is_approximate_position(curtain.current_position, 0))
// 		curtain.current_position = 0;
// 	else if(GPIO::is_open() && !!is_approximate_position(curtain.current_position, curtain.length))
// 		curtain.current_position = curtain.length;
// }


// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

bool current_position_is_practically_desire_position(Curtain& curtain)
{
	//TODO: figure out when 
	if(GPIO::is_open() || GPIO::is_closed())
	if(is_approximate_position(curtain.current_position, !is_closed() ))
	return is_approximate_position(curtain.desired_position, curtain.current_position);
}

