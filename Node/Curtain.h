
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.28                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Main Curtain class for organizing received data into an object. Also contains abstracted curtain      *
*    related functions such as what state a position would be.                                                         *
*   BUGS:                                                                                                              *
*   FUTURE: - Add cool temperature, light, & thermostat data.                                                          *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include "Global.h"
#include "Gpio.h"
#include "Transmission.h"
#include "User.h"


namespace Curtain
{
	bool is_approximate_position(uint32_t, uint32_t);
	bool is_approximate_position(uint32_t, uint32_t, uint32_t);
	CurtainState approximate_state_of(uint32_t, uint32_t);
	CurtainState state_of(uint32_t, uint32_t);

	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	// Position1 is within Gobal::wiggle_room of position2.
	// Takes two step positions.
	// Returns whether they are within a certain amount of eachother.
	bool is_approximate_position(uint32_t position1, uint32_t position2)
	{
		return is_approximate_position(position1, position2, Global::wiggle_room);
	}


	// Position1 is within an allowable difference of position2.
	// Takes two step positions, and an allowable difference.
	// Returns whether they are within a certain amount of eachother.
	bool is_approximate_position(uint32_t position1, uint32_t position2, uint32_t allowable_difference)
	{
		return (position1 - allowable_difference <= position2) && (position2 <= position1 + allowable_difference);
	}


	// Approximates the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the approximated current state for position.
	CurtainState approximate_state_of(uint32_t position, uint32_t curtain_length)
	{
		if(is_approximate_position(position, 0)) return CLOSED;
		if(is_approximate_position(position, curtain_length)) return OPEN;
		return MIDDLE;
	}


	// Determines the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the current state for position.
	CurtainState state_of(uint32_t position, uint32_t curtain_length)
	{
		if(position == 0) return CLOSED;
		if(position == curtain_length) return OPEN;
		return MIDDLE;
	}


	// ————————————————————————————————————————————————————— CLASS —————————————————————————————————————————————————————

	// Reads base128 data points from packet byte array.
	// Takes the location of the packet array.
	// Substracts the added 1 from each byte. Bit shifts each part of the base128 number to its corresponding part in 
	// the uint32_t number. Places segment into uint32_t parts for object.
	Curtain::Curtain(byte packet_buffer[])
	{
		_current_position = Json::value_for_key((char*)packet_buffer, Transmission::CURRENT_POS_KEY);
		_length = Json::value_for_key((char*)packet_buffer, Transmission::LENGTH_KEY);

		_event = Json::value_for_key((char*)packet_buffer, Transmission::EVENT_KEY);
		_desired_position = Json::value_for_key((char*)packet_buffer, Transmission::DESIRED_POS_KEY);

		_direction = Json::value_for_key((char*)packet_buffer, Transmission::DIRECTION_KEY);
		_auto_calibrate = Json::value_for_key((char*)packet_buffer, Transmission::CALIBRATE_KEY);
		_auto_correct = Json::value_for_key((char*)packet_buffer, Transmission::CORRECT_KEY);
	}


	// Writes relevent data to packet array.
	// Takes location of packet array, the location of curtain (to prevent needing to copy).
	// Sets current position and length in base 128 + 1 equivalent values.
	// NOTES: when C_String::length is used, the previous skipped value is added to the buffer pointer so that it does
	//   not need to retraverse recount the precalculated string literal changes.
	void Curtain::encode(byte packet_buffer[])
	{
		packet_buffer[0] = '{';
		// current position
		C_String::copy(Transmission::CURRENT_POS_KEY, (char*)packet_buffer+1);  // +2 from previous ", "
		packet_buffer += sizeof(Transmission::CURRENT_POS_KEY);  // -1 + 1 (for ignore NULL Terminator & start '{')
		C_String::copy_n(" : ", (char*)packet_buffer, 3);
		C_String::itoa(_current_position, (char*)packet_buffer+3);  // +3 from previous " : "
		packet_buffer += C_String::length((char*)packet_buffer+3) + 3;  // move packet_buffer to next NULL Terminator
		C_String::copy_n(", ", (char*)packet_buffer, 2);
		// event
		C_String::copy(Transmission::EVENT_KEY, (char*)packet_buffer+2);  // +2 from previous ", "
		packet_buffer += sizeof(Transmission::EVENT_KEY) + 1;  // -1 + 2 (for ignore NULL Terminator & add ", ")
		C_String::copy_n(" : ", (char*)packet_buffer, 3);
		C_String::itoa(_event, (char*)packet_buffer+3);  // +3 from previous " : "
		packet_buffer += C_String::length((char*)packet_buffer+3) + 3;  // move packet_buffer to next NULL Terminator
		C_String::copy_n(", ", (char*)packet_buffer, 2);
		// length
		C_String::copy(Transmission::LENGTH_KEY, (char*)packet_buffer+2);  // +2 from previous ", "
		packet_buffer += sizeof(Transmission::LENGTH_KEY) + 1;  // -1 + 2 (for ignore NULL Terminator & add ", ")
		C_String::copy_n(" : ", (char*)packet_buffer, 3);
		C_String::itoa(_length, (char*)packet_buffer+3);  // +3 from previous " : "
		packet_buffer += C_String::length((char*)packet_buffer+3) + 3;  // move packet_buffer to next NULL Terminator

		*packet_buffer = '}';
		packet_buffer[1] = 0;
	}


	// —————————————————————————————————————————— CLASS::GETTERS: ATTRIBUTES ——————————————————————————————————————————

	bool Curtain::calibrate()
	{
		return _auto_calibrate;
	}


	bool Curtain::correct()
	{
		return _auto_correct;
	}


	bool Curtain::direction()
	{
		return _direction;
	}


	uint32_t Curtain::current_position()
	{
		return _current_position;
	}


	uint32_t Curtain::length()
	{
		return _length;
	}


	uint32_t Curtain::desired_position()
	{
		return _desired_position;
	}


	uint32_t Curtain::event()
	{
		return _event;
	}


	// ————————————————————————————————————————————— CLASS::GETTERS: DATA —————————————————————————————————————————————

	bool Curtain::event_moves_to_an_end()
	{
		return _desired_position == 0 || _desired_position == _length;
	}


	// Determines whether the curtain moves all the way across the rod (open to close) for desired position.
	// Get the state of the curtain based of GPIO.  Compares with the state of the desired position.
	// Returns true if curtain moves all the way across rod, false otherwise.
	bool Curtain::moves_full_span()
	{
		CurtainState curtian_state = Gpio::state();
		CurtainState desired_state = state_of(_desired_position, _length);
		// parens not needed (precedence) but used to remove warnings
		return (curtian_state == CLOSED && desired_state == OPEN) || (curtian_state == OPEN && desired_state == CLOSED);
	}


	// SUGAR: whether the curtain should calibrate from moving the full span.
	bool Curtain::should_calibrate_across()
	{
		return calibrate() && moves_full_span();
	}


	// SUGAR: whether current position is open/close/middle.
	CurtainState Curtain::state_of_current_position()
	{
		return state_of(_current_position, _length);
	}


	// SUGAR: whether desired position is open/close/middle.
	CurtainState Curtain::state_of_desired_position()
	{
		return state_of(_desired_position, _length);
	}


	// —————————————————————————————————————————— CLASS::SETTERS: ATTRIBUTES ——————————————————————————————————————————

	void Curtain::current_position(uint32_t current_position)
	{
		_current_position = current_position;	
	}


	void Curtain::desired_position(uint32_t desired_position)
	{
		_desired_position = desired_position;	
	}


	void Curtain::length(uint32_t length)
	{
		_length = length;	
	}



	// ————————————————————————————————————————————— CLASS::SETTERS: DATA —————————————————————————————————————————————

	// Corrects position for DB unknowns relative to sensors.
	// Sets self::_current_position to match open/closed if applicable.
	void Curtain::set_current_position_if_does_not_match_sensors()
	{
		if(Gpio::is_closed() && !is_approximate_position(_current_position, 0))
			_current_position = 0;
		else if(Gpio::is_open() && !is_approximate_position(_current_position, _length))
			_current_position = _length;
	}


	// ASSUMES _desired_position WAS REACHED IF NOT AT AN END. COULD BE WRONG.
	// Sets the location of the curtain based on GPIO if possible, other wise desired location.
	void Curtain::set_location()
	{
		if(Gpio::is_open()) _current_position = _length;
		else if(Gpio::is_closed()) _current_position = 0;
		else _current_position = _desired_position;
	}

} // end namespace Curtain
