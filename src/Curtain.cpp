

#include "../Headers/Curtain.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/Message.hpp"


namespace Curtain
{
	using namespace Movement::CurtainStates;


	bool validate(DeserializedJSON& curtain_json)
	{
		using namespace Exception;
		using namespace Message::Literal::JSON::Key;

		// Validate structure
		if(!curtain_json.containsKey(CURTAIN_ID) || !curtain_json[CURTAIN_ID].is<int>())
		{
			new Exception(__LINE__, __FILE__, invalid_key_message(CURTAIN_ID, "int"));
			return false;
		}

		else if(curtain_json.containsKey(HOME_ID) && !curtain_json[HOME_ID].is<int>())
		{
			new Exception(__LINE__, __FILE__, invalid_key_message(HOME_ID, "int"));
			return false;
		}

		else if(curtain_json.containsKey(ROOM_ID) && !curtain_json[ROOM_ID].is<int>())
		{
			new Exception(__LINE__, __FILE__, invalid_key_message(ROOM_ID, "int"));
			return false;
		}

		// Validate hardware overriding values
		else if(curtain_json.containsKey(LENGTH) && !curtain_json[LENGTH].is<int>())
		{
			new Exception(__LINE__, __FILE__, invalid_key_message(LENGTH, "int"));
			return false;
		}

		// Validate movement overriding values
		else if(curtain_json.containsKey(AUTO_CORRECT) && !curtain_json[AUTO_CORRECT].is<bool>())
		{
			new Exception(__LINE__, __FILE__, invalid_key_message(AUTO_CORRECT, "bool"));
			return false;
		}

		return true;
	}


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	Curtain::Curtain()
	{}


	// —————————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————————— GETTERS::STRUCTURE ————————————————————————————————————————————— //

	uint16_t Curtain::id()
	{
		return _id;
	}


	uint16_t Curtain::room_id()
	{
		return _room_id;
	}


	uint16_t Curtain::home_id()
	{
		return _home_id;
	}


	// —————————————————————————————————————————————— GETTERS::OPTIONS —————————————————————————————————————————————— //

	bool Curtain::auto_correct()
	{
		return _auto_correct;
	}


	// ————————————————————————————————————————————— GETTERS::HARDWARE  ————————————————————————————————————————————— //

	uint32_t Curtain::length()
	{
		return _length;
	}


	bool Curtain::direction()
	{
		return _direction;
	}


	bool Curtain::is_moving()
	{
		return _is_moving;
	}


	uint32_t Curtain::percentage()
	{
		return _position * 100 / _length;
	}


	// ——————————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————————— //

	// CurtainState Curtain::state()
	// {
	// 	if(Hardware::state() == CLOSED)
	// 	{
	// 		_position = 0;
	// 	}
	// 	else if(_position == 0)
	// 	{
	// 		_position = _length;
	// 	}

	// 	if(_position == 0)
	// 	{
	// 		return CLOSED;
	// 	}
	// 	else if(_position == _length)
	// 	{
	// 		return FULLY_OPEN;
	// 	}

	// 	return PARTIALLY_OPEN;
	// }


	Curtain::operator String()
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject curtain_object = json_document.to<JsonObject>();

		curtain_object[Message::Literal::JSON::Key::CURTAIN_ID] = _id;
		curtain_object[Message::Literal::JSON::Key::ROOM_ID] = _room_id;
		curtain_object[Message::Literal::JSON::Key::HOME_ID] = _home_id;
		curtain_object[Message::Literal::JSON::Key::IS_MOVING] = _is_moving;
		curtain_object[Message::Literal::JSON::Key::LENGTH] = _length;
		curtain_object[Message::Literal::JSON::Key::PERCENTAGE] = percentage();
		curtain_object[Message::Literal::JSON::Key::AUTO_CORRECT] = _auto_correct;

		return Message::convert_JsonObject_to_String(curtain_object);
	}


	inline String invalid_key_message(const char* key, const char* type_str)
	{
		return String("Curtain object must contain key '") + key + "' of type '" + type_str + "'";
	}


	// —————————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————————— SETTERS::STRUCTURE ————————————————————————————————————————————— //

	void Curtain::room_id(uint16_t new_room_id)
	{
		_room_id = new_room_id;
	}


	void Curtain::home_id(uint16_t new_home_id)
	{
		_home_id = new_home_id;
	}


	// ————————————————————————————————————————————— SETTERS::HARDWARE  ————————————————————————————————————————————— //

	void Curtain::direction(bool new_direction)
	{
		_direction = new_direction;
	}


	void Curtain::length(uint32_t new_length)
	{
		_length = new_length;
	}


	void Curtain::is_moving(bool new_is_moving)
	{
		_is_moving = new_is_moving;
	}


	void Curtain::percentage(uint32_t new_percentage)
	{
		_position = new_percentage * _length / 100;
	}


	// —————————————————————————————————————————————— SETTERS::OPTIONS —————————————————————————————————————————————— //

	void Curtain::auto_correct(bool new_auto_correct)
	{
		_auto_correct = new_auto_correct;
	}


	void Curtain::update()
	{
		if(Hardware::state() == CLOSED)
		{
			_position = 0;
		}
		else if(_position == 0)
		{
			_position = _length;
		}
	}


	// ——————————————————————————————————————————————————————— OTHER  ——————————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	Curtain& Curtain::operator=(DeserializedJSON& update_json)
	{
		using namespace Message::Literal::JSON::Key;

		if(update_json.containsKey(HOME_ID))
		{
			_home_id = update_json[HOME_ID];
		}

		if(update_json.containsKey(ROOM_ID))
		{
			_room_id = update_json[ROOM_ID];
		}

		// Validate hardware overriding values
		if(update_json.containsKey(LENGTH))
		{
			_length = update_json[LENGTH];
		}

		// Validate movement overriding values
		if(update_json.containsKey(AUTO_CORRECT))
		{
			_auto_correct = update_json[AUTO_CORRECT];
		}
	}
}
