
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


namespace Curtain
{
	bool is_approximate_position(uint32_t, uint32_t);
	bool is_approximate_position(uint32_t, uint32_t, uint32_t);
	CurtainState approximate_state_of(uint32_t, uint32_t);
	CurtainState state_of(uint32_t, uint32_t);

	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	// Position1 is within an allowable difference of position2.
	// Takes two step positions, and an allowable difference.
	// Returns whether they are within a certain amount of eachother.
	bool is_approximate_position(uint32_t position1, uint32_t position2, uint32_t allowable_difference)
	{
		return (position1 - allowable_difference <= position2) && (position2 <= position1 + allowable_difference);
	}


	// Position1 is within Gobal::wiggle_room of position2.
	// Takes two step positions.
	// Returns whether they are within a certain amount of eachother.
	bool is_approximate_position(uint32_t position1, uint32_t position2)
	{
		return is_approximate_position(position1, position2, Global::wiggle_room);
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


	// ——————————————————————————————————————————————————— CLASS  ——————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ———————————————————————————————————————————————— CLASS::EVENT ———————————————————————————————————————————————— //

	Events::Event(uint32_t id, uint32_t curtain_length, uint32_t desired_position)
	{
		_id = id;
		_curtain_length = curtain_length;
		_desired_position = desired_position;
	}


	uint32_t Event::id()
	{
		return _id;
	}


	uint32_t Event::desired_position()
	{
		return _desired_position;
	}


	bool Event::event_moves_to_an_end()
	{
		return _desired_position == 0 || _desired_position == _curtain_length;
	}


	// Determines whether the curtain moves all the way across the rod (open to close) for desired position.
	// Get the state of the curtain based of GPIO.  Compares with the state of the desired position.
	// Returns true if curtain moves all the way across rod, false otherwise.
	bool Curtain::moves_full_span()
	{
		CurtainState curtian_state = Gpio::state();
		CurtainState desired_state = state_of(_desired_position, _curtain_length);
		// parens not needed (precedence) but used to remove warnings
		return (curtian_state == CLOSED && desired_state == OPEN) || (curtian_state == OPEN && desired_state == CLOSED);
	}


	// SUGAR: whether desired position is open/close/middle.
	CurtainState Event::state_of_desired_position()
	{
		return state_of(_desired_position, _curtain_length);
	}


	// ——————————————————————————————————————————————— CLASS::CURTAIN ——————————————————————————————————————————————— //

	// Reads base128 data points from packet byte array.
	// Takes the location of the packet array.
	// Substracts the added 1 from each byte. Bit shifts each part of the base128 number to its corresponding part in 
	// the uint32_t number. Places segment into uint32_t parts for object.
	Curtain::Curtain(StaticJsonDocument<Global::JSON_BUFFER_SIZE>& json)
	: _event{Event(
		json[Transmission::Literals::JSON::Key::EVENT][Transmission::Literals::JSON::Key::EVENT],
		json[Transmission::Literals::JSON::Key::CURTAIN][Transmission::Literals::JSON::Key::CURTAIN_ID],
		json[Transmission::Literals::JSON::Key::EVENT][Transmission::Literals::JSON::Key::EVENT_DESIRED_POS]
	)}
	{
		uint16_t curtain_id = json[Transmission::Literals::JSON::Key::CURTAIN]
		  [Transmission::Literals::JSON::Key::CURTAIN_ID];
		if(curtain_id != C_String::atoi(Configure::Curtain::curtain_id))
		{
			Exceptions::throw_generic("Message sent to wrong curtain");
		}

		_current_position = json[Transmission::Literals::JSON::Key::CURRENT_POS];
		_length = json[Transmission::Literals::JSON::Key::LENGTH];

		_direction = json[Transmission::Literals::JSON::Key::DIRECTION];
		_is_smart = json[Transmission::Literals::JSON::Key::IS_SMART];
		if(_is_smart)
		{
			_auto_calibrate = json[Transmission::Literals::JSON::Key::CALIBRATE];
			_auto_correct = json[Transmission::Literals::JSON::Key::CORRECT];
		}
	}


	// Writes relevent data to packet array.
	// Takes location of packet array, the location of curtain (to prevent needing to copy).
	// Sets current position and length in base 128 + 1 equivalent values.
	// NOTES: when C_String::length is used, the previous skipped value is added to the buffer pointer so that it does
	//   not need to retraverse recount the precalculated string literal changes.
	char* Curtain::serialize_data()
	{
		char* buffer_head = (char*)malloc(Global::JSON_BUFFER_SIZE), *buffer = buffer_head;

		C_String::copy_n("{\"", buffer, 2);
		// current position
		C_String::copy(Transmission::CURRENT_POS_KEY, buffer+2);  // +2 from previous "{\""
		buffer += sizeof(Transmission::CURRENT_POS_KEY)+1;  // -1 + 2 (for ignore NULL Terminator & start "{\"")
		C_String::copy_n("\" : ", buffer, 4);
		C_String::itoa(_current_position, buffer+4);  // +4 from previous "\" : "
		buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		C_String::copy_n(", \"", buffer, 3);
		// curtain
		C_String::copy(Transmission::CURTAIN_KEY, buffer+3);  // +3 from previous ", \""
		buffer += sizeof(Transmission::CURTAIN_KEY) + 2;  // -1 + 3 (for ignore NULL Terminator & add ", \"")
		C_String::copy_n("\" : ", buffer, 4);
		C_String::copy(Configure::Curtain::curtain_id, buffer+4);  // +4 from previous "\" : "
		buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		C_String::copy_n(", \"", buffer, 3);
		// event
		C_String::copy(Transmission::EVENT_KEY, buffer+3);  // +3 from previous ", \""
		buffer += sizeof(Transmission::EVENT_KEY) + 2;  // -1 + 3 (for ignore NULL Terminator & add ", \"")
		C_String::copy_n("\" : ", buffer, 4);
		C_String::itoa(_event, buffer+4);  // +4 from previous "\" : "
		buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		C_String::copy_n(", \"", buffer, 3);
		// length
		C_String::copy(Transmission::LENGTH_KEY, buffer+3);  // +3 from previous ", \""
		buffer += sizeof(Transmission::LENGTH_KEY)+2;  // -1 + 3 (for ignore NULL Terminator & add ", \"")
		C_String::copy_n("\" : ", buffer, 4);
		C_String::itoa(_length, buffer+4);  // +4 from previous " : "
		buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		// finish json
		C_String::copy_n("}", buffer, 1);

		return buffer_head;
	}


	void Curtain::move()
	{

		if(!curtain.is_smart()) Gpio::move(curtain);
		else
		{
			if(!curtain.event_moves_to_an_end()) Gpio::move(curtain);
			// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
			// ing or closing curtain.
			// Also does not take into account if desire == current.  It can be 'move 0' or ignored by Master.
			else
			{
				if(curtain.should_calibrate_across()) curtain.length(Gpio::calibrate_to_opposite(curtain.direction()));
				else if(curtain.state_of_desired_position() == Curtain::OPEN) Gpio::move_until_open(curtain.direction());
				else Gpio::move_until_closed(curtain.direction());
			}
		}
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


	uint32_t Curtain::current_position()
	{
		return _current_position;
	}


	uint32_t Curtain::event()
	{
		return _event;
	}


	bool Curtain::is_smart()
	{
		return _is_smart;
	}


	uint32_t Curtain::length()
	{
		return _length;
	}


	// ————————————————————————————————————————————— CLASS::GETTERS: DATA —————————————————————————————————————————————

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
		if(!_is_smart)
		{
			Global::current_position = _desired_position;
			_current_position = _desired_position;  // curtain isn't that smart, so guess where it is
		}
		else
		{
			if(Gpio::is_open()) _current_position = _length;
			else if(Gpio::is_closed()) _current_position = 0;
			else _current_position = _desired_position;  // curtain isn't that smart, so guess where it is
			Global::current_position = _current_position;
		}
	}


	// ————————————————————————————————————————————————— CLASS::WRITE —————————————————————————————————————————————————

	void Curtain::send_hub_serialized_info()
	{
		char* serialized_data = serialize_data();
		Transmission::update_hub((byte*)serialized_data);
		delete serialized_data;
	}


} // end namespace Curtain
