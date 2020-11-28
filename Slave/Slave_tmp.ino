
/***********************************************************************************************************
*
*	created by: MPZinke
*	on 2020.09.19
*
*	DESCRIPTION: Ethernet connection version of smart curtain.  Queries server and waits
*		on response for next event.  On event, drives curtain to location.  
*		**Main arduino file**
*	BUGS:
*	FUTURE:	- Add cool temperature, light, & thermostat data.
*
***********************************************************************************************************/

#include <SPI.h>
#include <Ethernet.h>

#include "Global.h"


byte MAC[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}
IPAddress SERVER(10,0,0,11);  // HARDCODED: connection endpoint from where response is saught
EthernetClient CLIENT;  // the magician


void setup()
{
	pinMode(CLOSE_PIN, INPUT);
	pinMode(OPEN_PIN, INPUT);
	pinMode(DIRECTION_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);
	pinMode(PULSE_PIN, OUTPUT);

	// setup ethernet
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	while(!Ethernet.begin(MAC)) delay(1000);  // wait while not connected to LAN
	while(!CLIENT.connect(SERVER, 80)) delay(1000);  // wait while not connected to device

	delay(LOOP_WAIT);
}


void loop()
{
	// check for next event & move for it
	if(queued_event(&curtain, &event))
	{
		if(event_moves_to_an_end(event.position, curtain.length))
		{
			if(curtain.calibrate && currently_at_an_end()) // return_len(move_to_end(curtain.direction))
			else if(event.position <= WIGGLE_ROOM) move_until_closed(curtain.direction);
			else move_until_open(curtain.direction);
		}
		else if(curtain.correct)
		{
			// move to position & correct if wrong
		}
		else
		{
			// just move: no fancy stuff
		}


	}
	// update current location
	else
	{
		uint8_t current_pin = current_activated_pin();
		if(current_pin == CLOSE_PIN) set_curtain_as_closed();
		else if(current_pin == OPEN_PIN) set_curtain_as_open();
		else set_curtain_as_partially_open();
	}
	// 

}


// ————————————————— GET EVENTS ——————————————————

// Get possible event that has passed.
// Takes location to CURTAIN, EVENT to write data to if valid.
// Checks if valid. If so, populates structs.
// Return whether received package is valid.
bool queued_event(CURTAIN* curtain, EVENT* event)
{
	byte buffer[BUFFER_SIZE];
	if(!read_data(buffer)) return false;

	*curtain = (CURTAIN)	{
								buffer[MODES_INDEX] & CALIBRATE_MASK,
								buffer[MODES_INDEX] & CORRECT_MASK,
								buffer[MODES_INDEX] & DIRECTION_MASK,
								value_for_next_three_bytes(buffer, C_LENGTH_INDEX), 
								value_for_next_three_bytes(buffer, C_POSITION_INDEX)
							};

	*event = (EVENT)	{
							value_for_next_three_bytes(buffer, E_ID_INDEX),
							value_for_next_three_bytes(buffer, E_POSITION_INDEX)
						};
	return true;
}


// Converts next 3 bytes into single 32 bit int.
// Takes buffer array of scraped data, starting offset of value.
// Takes each byte sequentially & masks it into its corresponding 7 bit (2^7=128) section.
// Returns 32 bit int mapped value.
uint32_t value_for_next_three_bytes(byte buffer[], uint8_t start)
{
	return ((buffer[start]-1) << 14) | ((buffer[start+1]-1) << 7) | (buffer[start+2]-1);
}


// —————————————————— UTILITY ———————————————————

bool event_moves_to_an_end(uint32_t destination, uint32_t length)
{
	return destination <= WIGGLE_ROOM || length - WIGGLE_ROOM <= destination;  // desires closed || open
}


bool currently_at_an_end()
{
	return !digitalRead(CLOSE_PIN) || !digitalRead(OPEN_PIN);
}


bool direction(long current_position, long event_position, bool DB_move_direction)
{
	return (0 < (current_position - event_position)) ^ DB_move_direction;
}


bool direction(char current_pin, bool DB_move_direction)
{
	return (current_pin == OPEN_PIN) ^ DB_move_direction;
}


long needed_steps(long current_position, long event_position)
{
	return labs(event_position - current_position);
}


long needed_steps(char current_pin, long curtain_length, long event_position)
{
	if(current_pin == CLOSE_PIN) return event_position;
	return curtain_length - event_position;
}