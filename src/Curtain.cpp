
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.10                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Curtain.hpp"

#include "../Headers/Global.hpp"
#include "../Headers/C_String.hpp"
#include "../Headers/Exceptions.hpp"
#include "../Headers/Transmission.hpp"


namespace Curtain
{
	using namespace Movement::CurtainStates;


	// ——————————————————————————————————————————————————— CLASS  ——————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ——————————————————————————————————————————————— CLASS::CURTAIN ——————————————————————————————————————————————— //

	Curtain::Curtain(bool initialize)
	{
		_auto_calibrate = CLOSE_ENDSTOP && OPEN_ENDSTOP;
		_auto_correct = CLOSE_ENDSTOP || OPEN_ENDSTOP;
		_discrete_movement = CLOSE_ENDSTOP || OPEN_ENDSTOP;
		_direction = Config::Hardware::DIRECTION_SWITCH;

		_length = Config::Hardware::DEFAULT_LENGTH;
		_position = Movement::current_state() == MIDDLE ? _length / 2 : Movement::current_state() * _length;
	}


	// Writes relevent data to packet array.
	// Takes location of packet array, the location of curtain (to prevent needing to copy).
	// Sets current position and length in base 128 + 1 equivalent values.
	// NOTES: when C_String::length is used, the previous skipped value is added to the buffer pointer so that it does
	//   not need to retraverse recount the precalculated string literal changes.
	char* Curtain::serialized_data()
	{
		char* buffer_head = (char*)malloc(JSON_BUFFER_SIZE), *buffer = buffer_head;
		// TODO: Use JSON library
		// C_String::copy_n("{\"", buffer, 2);
		// // current position
		// C_String::copy(Transmission::Literal::JSON::Key::CURTAIN_POSITION, buffer+2);  // +2 from previous "{\""
		// buffer += sizeof(Transmission::Literal::JSON::Key::CURTAIN_POSITION)+1;  // -1 + 2 (for ignore NULL Terminator & start "{\"")
		// C_String::copy_n("\" : ", buffer, 4);
		// C_String::itoa(_position, buffer+4);  // +4 from previous "\" : "
		// buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		// C_String::copy_n(", \"", buffer, 3);
		// // curtain
		// C_String::copy(Transmission::Literal::JSON::Key::CURTAIN, buffer+3);  // +3 from previous ", \""
		// buffer += sizeof(Transmission::Literal::JSON::Key::CURTAIN) + 2;  // -1 + 3 (for ignore NULL Terminator & add ", \"")
		// C_String::copy_n("\" : ", buffer, 4);
		// C_String::copy(Config::Curtain::CURTAIN_ID, buffer+4);  // +4 from previous "\" : "
		// buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		// C_String::copy_n(", \"", buffer, 3);
		// // event
		// C_String::copy(Transmission::Literal::JSON::Key::EVENT, buffer+3);  // +3 from previous ", \""
		// buffer += sizeof(Transmission::Literal::JSON::Key::EVENT) + 2;  // -1 + 3 (for ignore NULL Terminator & add ", \"")
		// C_String::copy_n("\" : ", buffer, 4);
		// C_String::itoa(_event.id(), buffer+4);  // +4 from previous "\" : "
		// buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		// C_String::copy_n(", \"", buffer, 3);
		// // length
		// C_String::copy(Transmission::Literal::JSON::Key::LENGTH, buffer+3);  // +3 from previous ", \""
		// buffer += sizeof(Transmission::Literal::JSON::Key::LENGTH)+2;  // -1 + 3 (for ignore NULL Terminator & add ", \"")
		// C_String::copy_n("\" : ", buffer, 4);
		// C_String::itoa(_length, buffer+4);  // +4 from previous " : "
		// buffer += C_String::length(buffer+4) + 4;  // move buffer to '\0'; ((+4) + 4) to skip counting redundant chars
		// // finish json
		// C_String::copy_n("}", buffer, 1);

		return buffer_head;
	}


	// void Curtain::move()
	// {
		
	// 	if(!curtain.is_smart()) Movement::move(curtain);
	// 	else
	// 	{
	// 		if(!_event.event_moves_to_an_end()) Movement::move(curtain);
	// 		// Does not take into account if actual position does not match 'current', b/c this can be reset by fully open-
	// 		// ing or closing curtain.
	// 		// Also does not take into account if desire == current.  It can be 'move 0' or ignored by Master.
	// 		else
	// 		{
	// 			if(should_auto_calibrate_across()) _length(Movement::auto_calibrate_to_opposite(inline direction()));
	// 			else if(inline state_of_position() == Curtain::OPEN) Movement::move_until_open(inline direction());
	// 			else Movement::move_until_closed(inline direction());
	// 		}
	// 	}
	// }

	bool Curtain::auto_calibrate()
	{
		return _auto_calibrate;
	}


	bool Curtain::auto_correct()
	{
		return _auto_correct;
	}


	bool Curtain::direction()
	{
		return _direction;
	}


	bool Curtain::discrete_movement()
	{
		return _discrete_movement;
	}


	uint32_t Curtain::length()
	{
		return _length;
	}


	uint32_t Curtain::position()
	{
		return _position;
	}



	// —————————————————————————————————————————— CLASS::SETTERS: ATTRIBUTES ——————————————————————————————————————————

	
	void Curtain::auto_calibrate(bool new_auto_calibrate)
	{
		_auto_calibrate = new_auto_calibrate;
	}


	void Curtain::auto_correct(bool new_auto_correct)
	{
		_auto_correct = new_auto_correct;
	}


	void Curtain::direction(bool new_direction)
	{
		_direction = new_direction;
	}


	void Curtain::length(uint32_t new_length)
	{
		_length = new_length;
	}


	void Curtain::discrete_movement(bool new_discrete_movement)
	{
		_discrete_movement = new_discrete_movement;
	}


	void Curtain::position(uint32_t new_position)
	{
		_position = new_position;
	}


	void Curtain::update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		JsonObject curtain_object = json_document[Transmission::Literal::JSON::Key::CURTAIN];

		if(curtain_object.containsKey(Transmission::Literal::JSON::Key::AUTO_CALIBRATE))
		{
			_auto_calibrate = curtain_object[Transmission::Literal::JSON::Key::AUTO_CALIBRATE];
		}

		if(curtain_object.containsKey(Transmission::Literal::JSON::Key::AUTO_CORRECT))
		{
			_auto_correct = curtain_object[Transmission::Literal::JSON::Key::AUTO_CORRECT];
		}

		if(curtain_object.containsKey(Transmission::Literal::JSON::Key::DIRECTION))
		{
			_direction = curtain_object[Transmission::Literal::JSON::Key::DIRECTION];
		}

		if(curtain_object.containsKey(Transmission::Literal::JSON::Key::DISCRETE_MOVEMENT))
		{
			_discrete_movement = curtain_object[Transmission::Literal::JSON::Key::DISCRETE_MOVEMENT];
		}

		if(curtain_object.containsKey(Transmission::Literal::JSON::Key::LENGTH))
		{
			_length = curtain_object[Transmission::Literal::JSON::Key::LENGTH];
		}

		if(curtain_object.containsKey(Transmission::Literal::JSON::Key::CURTAIN_POSITION))
		{
			_position = curtain_object[Transmission::Literal::JSON::Key::CURTAIN_POSITION];
		}
	}



	// ————————————————————————————————————————————— CLASS::SETTERS: DATA —————————————————————————————————————————————

	// // Corrects position for DB unknowns relative to sensors.
	// // Sets self::_position to match open/closed if applicable.
	// void Curtain::set_position_if_does_not_match_sensors()
	// {
	// 	if(Movement::is_closed() && !is_approximate_position(_position, 0))
	// 		_position = 0;
	// 	else if(Movement::is_open() && !is_approximate_position(_position, _length))
	// 		_position = _length;
	// }


	// // ASSUMES _desired_position WAS REACHED IF NOT AT AN END. COULD BE WRONG.
	// // Sets the location of the curtain based on GPIO if possible, other wise desired location.
	// void Curtain::set_location()
	// {
	// 	if(!_is_smart)
	// 	{
	// 		Global::current_position = _desired_position;
	// 		_position = _desired_position;  // curtain isn't that smart, so guess where it is
	// 	}
	// 	else
	// 	{
	// 		if(Movement::is_open()) _position = _length;
	// 		else if(Movement::is_closed()) _position = 0;
	// 		else _position = _desired_position;  // curtain isn't that smart, so guess where it is
	// 		Global::current_position = _position;
	// 	}
	// }


	// ————————————————————————————————————————————————— CLASS::WRITE —————————————————————————————————————————————————

	void Curtain::send_hub_serialized_info()
	{
		char* serialized_data_c_str = serialized_data();
		Transmission::update_hub((byte*)serialized_data_c_str);
		delete[] serialized_data_c_str;
	}

}