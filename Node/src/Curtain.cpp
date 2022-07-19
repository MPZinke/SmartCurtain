
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

#include "../Headers/Exceptions.hpp"
#include "../Headers/Hardware.hpp"
#include "../Headers/Request.hpp"


namespace Curtain
{
	using namespace Movement::CurtainStates;


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //

	Curtain::Curtain(uint8_t id)
	: _id{id}
	{
		using namespace Config::Hardware;

		_auto_calibrate = BOTH_ENDSTOPS;
		_auto_correct = EITHER_ENDSTOP;
		_discrete_movement = EITHER_ENDSTOP && DISCRETE_MOVEMENT_ALLOWED;
		_direction = DIRECTION_SWITCH;

		_length = DEFAULT_LENGTH;

		register CurtainState curtain_state = Hardware::current_hardware_state();
		// If known open or unknown the curtain can detect when it closes
		if(curtain_state == OPEN || (curtain_state == UNKNOWN && CLOSE_ENDSTOP))
		{
			_position = _length;
		}
		else if(curtain_state == MIDDLE)
		{
			_position = _length / 2;
		}
		// If known closed or unknown the curtain can detect when it open
		// else if(curtain_state == CLOSED || (curtain_state == UNKNOWN && OPEN_ENDSTOP) || curtain_state != MIDDLE)
		else
		{
			_position = 0;
		}

		_percentage = _length ? _position * 100 / _length : 0;
	}


	// FREE ME WHEN DONE
	// SUMMARY: Creates a malloced char array the size of the serialized json and writes it.
	// DETAILS: Called when a Curtain object is attempted to be converted to a char*. Converts object to a JsonObject.
	//  Mallocs char* array for c_string. Serializes data to c_string.
	Curtain::operator String()
	{
		using namespace Request::Literal;  // not entire namespace to help show where the below values are from
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject curtain_object = json_document.to<JsonObject>();

		curtain_object[JSON::Key::CURTAIN_ID] = _id;
		curtain_object[JSON::Key::AUTO_CALIBRATE] = _auto_calibrate;
		curtain_object[JSON::Key::AUTO_CORRECT] = _auto_correct;
		curtain_object[JSON::Key::DIRECTION] = _direction;
		curtain_object[JSON::Key::DISCRETE_MOVEMENT] = _discrete_movement;
		curtain_object[JSON::Key::LENGTH] = _length;
		curtain_object[JSON::Key::CURTAIN_PERCENTAGE] = _percentage;
		curtain_object[JSON::Key::CURTAIN_POSITION] = _position;

		return Request::convert_JsonObject_to_String(curtain_object);
	}


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

	uint8_t Curtain::id()
	{
		return _id;
	}


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


	uint8_t Curtain::percentage()
	{
		return _percentage;
	}


	uint32_t Curtain::position()
	{
		return _position;
	}


	CurtainState Curtain::state()
	{
		register CurtainState curtain_state;
		if((curtain_state = Hardware::current_hardware_state()) != UNKNOWN)
		{
			return curtain_state;
		}

		register uint8_t percentage = _percentage;
		if(!percentage) return CLOSED;
		if(percentage == 100) return OPEN;
		return MIDDLE;
	}


	// ——————————————————————————————————————————————————— SETTER ——————————————————————————————————————————————————— //
	
	void Curtain::auto_calibrate(register bool new_auto_calibrate)
	{
		using namespace Config::Hardware;

		if(new_auto_calibrate && !BOTH_ENDSTOPS)
		{
			return (void)new Exceptions::INTERNAL_SERVER_ERROR_500_Exception(__LINE__, __FILE__,
			  "Cannot enable auto calibration when an endstop is not configured");
		}
		_auto_calibrate = new_auto_calibrate;
	}


	void Curtain::auto_correct(register bool new_auto_correct)
	{
		using namespace Config::Hardware;

		if(new_auto_correct && !EITHER_ENDSTOP)
		{
			return (void)new Exceptions::INTERNAL_SERVER_ERROR_500_Exception(__LINE__, __FILE__,
			  "Cannot enable auto correcting when no endstop is not configured");
		}
		_auto_correct = new_auto_correct;
	}


	void Curtain::direction(register bool new_direction)
	{
		_direction = new_direction;
	}


	void Curtain::length(register uint32_t new_length)
	{
		_length = new_length;
	}


	void Curtain::discrete_movement(register bool new_discrete_movement)
	{
		using namespace Config::Hardware;

		if(new_discrete_movement && !(EITHER_ENDSTOP || DISCRETE_MOVEMENT_ALLOWED))
		{
			return (void)new Exceptions::INTERNAL_SERVER_ERROR_500_Exception(__LINE__, __FILE__,
			  "Cannot enable auto correcting when no endstop is not configured");
		}
		_discrete_movement = new_discrete_movement;
	}


	void Curtain::percentage(register uint8_t new_percentage)
	{
		_percentage = new_percentage;
		_position = ((uint32_t)new_percentage) * _length / 100;
	}


	void Curtain::position(register uint32_t new_position)
	{
		_position = new_position;
		_percentage = new_position * 100 / _length;  // multiply by 100 first to keep as much precision
	}


	// SUMMARY: Update attributes based on key-values of a json.
	// PARAMS: Takes the json document.
	// DETAILS: For every class attribute key that exists in the JSON, the class attribute is updated to the key's
	//  value.
	void Curtain::update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		using namespace Request::Literal;  // not entire namespace to help show where the below values are from
		using namespace Config::Hardware;

		JsonObject curtain_object = json_document[JSON::Key::CURTAIN];

		// Restrict auto_calibrate to hardware
		if(curtain_object.containsKey(JSON::Key::AUTO_CALIBRATE))
		{
			auto_calibrate(curtain_object[JSON::Key::AUTO_CALIBRATE]);
		}

		// Restrict auto_correct to hardware
		if(curtain_object.containsKey(JSON::Key::AUTO_CORRECT) && EITHER_ENDSTOP)
		{
			auto_correct(curtain_object[JSON::Key::AUTO_CORRECT]);
		}

		if(curtain_object.containsKey(JSON::Key::DIRECTION))
		{
			direction(curtain_object[JSON::Key::DIRECTION]);
		}

		if(curtain_object.containsKey(JSON::Key::DISCRETE_MOVEMENT))
		{
			discrete_movement(curtain_object[JSON::Key::DISCRETE_MOVEMENT]);
		}

		if(curtain_object.containsKey(JSON::Key::LENGTH))
		{
			length(curtain_object[JSON::Key::LENGTH]);
		}

		if(curtain_object.containsKey(JSON::Key::CURTAIN_POSITION))
		{
			position(curtain_object[JSON::Key::CURTAIN_POSITION]);
		}
	}


	// ——————————————————————————————————————————————————— OTHER ——————————————————————————————————————————————————— //

	void Curtain::append_to(JsonObject& json_object)
	{
		using namespace Request::Literal;  // not entire namespace to help show where the below values are from

		json_object[JSON::Key::CURTAIN][JSON::Key::CURTAIN_ID] = _id;
		json_object[JSON::Key::CURTAIN][JSON::Key::AUTO_CALIBRATE] = _auto_calibrate;
		json_object[JSON::Key::CURTAIN][JSON::Key::AUTO_CORRECT] = _auto_correct;
		json_object[JSON::Key::CURTAIN][JSON::Key::DIRECTION] = _direction;
		json_object[JSON::Key::CURTAIN][JSON::Key::DISCRETE_MOVEMENT] = _discrete_movement;
		json_object[JSON::Key::CURTAIN][JSON::Key::LENGTH] = _length;
		json_object[JSON::Key::CURTAIN][JSON::Key::CURTAIN_PERCENTAGE] = _percentage;
		json_object[JSON::Key::CURTAIN][JSON::Key::CURTAIN_POSITION] = _position;
	}

}  // end namespace Curtain
