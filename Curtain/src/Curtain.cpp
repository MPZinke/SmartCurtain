

#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/Message.hpp"
#include "../Headers/StaticString.hpp"


namespace Curtain
{
	using namespace Movement::CurtainStates;


	inline String invalid_key_message(const char* key, const char* type_str)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return String("Curtain object must contain key '") + key + "' of type '" + type_str + "'";
	}


	bool validate(DeserializedJSON::DeserializedJSON& curtain_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		using namespace Message::Literal::JSON::Key;

		// Validate structure
		if(!curtain_json.containsKey(CURTAIN_ID) || !curtain_json[CURTAIN_ID].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, invalid_key_message(CURTAIN_ID, "int"));
			return false;
		}

		else if(curtain_json.containsKey(HOME_ID) && !curtain_json[HOME_ID].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, invalid_key_message(HOME_ID, "int"));
			return false;
		}

		else if(curtain_json.containsKey(ROOM_ID) && !curtain_json[ROOM_ID].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, invalid_key_message(ROOM_ID, "int"));
			return false;
		}

		// Validate hardware overriding values
		else if(curtain_json.containsKey(LENGTH) && !curtain_json[LENGTH].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, invalid_key_message(LENGTH, "int"));
			return false;
		}

		// Validate movement overriding values
		else if(curtain_json.containsKey(AUTO_CORRECT) && !curtain_json[AUTO_CORRECT].is<bool>())
		{
			new Exception::Exception(__LINE__, __FILE__, invalid_key_message(AUTO_CORRECT, "bool"));
			return false;
		}

		return true;
	}


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	Curtain::Curtain()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{}


	// —————————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————————— GETTERS::STRUCTURE ————————————————————————————————————————————— //

	const char* Curtain::id() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _id;
	}


	const char* Curtain::room_id() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _room_id;
	}


	const char* Curtain::home_id() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _home_id;
	}


	// —————————————————————————————————————————————— GETTERS::OPTIONS —————————————————————————————————————————————— //

	bool Curtain::auto_correct() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _auto_correct;
	}


	// ————————————————————————————————————————————— GETTERS::HARDWARE  ————————————————————————————————————————————— //

	uint32_t Curtain::length() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _length;
	}


	bool Curtain::direction() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _direction;
	}


	bool Curtain::is_moving() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _is_moving;
	}


	uint32_t Curtain::percentage() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _position * 100 / _length;
	}


	// ——————————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————————— //

	Curtain::operator StaticString<JSON_BUFFER_SIZE>() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
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

		return StaticString<JSON_BUFFER_SIZE>(curtain_object);
	}


	// —————————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————————— SETTERS::STRUCTURE ————————————————————————————————————————————— //

	void Curtain::room_id(const char* new_room_id)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		StaticString<0>::copy(new_room_id, (char*)_room_id);
	}


	void Curtain::home_id(const char* new_home_id)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		StaticString<0>::copy(new_home_id, (char*)_home_id);
	}


	// ————————————————————————————————————————————— SETTERS::HARDWARE  ————————————————————————————————————————————— //

	void Curtain::direction(bool new_direction)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_direction = new_direction;
	}


	void Curtain::length(uint32_t new_length)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_length = new_length;
	}


	void Curtain::is_moving(bool new_is_moving)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_is_moving = new_is_moving;
	}


	void Curtain::percentage(uint32_t new_percentage)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_position = new_percentage * _length / 100;
	}


	// —————————————————————————————————————————————— SETTERS::OPTIONS —————————————————————————————————————————————— //

	void Curtain::auto_correct(bool new_auto_correct)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_auto_correct = new_auto_correct;
	}


	void Curtain::update()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(Hardware::state() == CLOSED)
		{
			_position = 0;
		}
		else if(_position == 0)
		{
			_position = _length;
		}
		// else { assume position is correct }
	}


	// ——————————————————————————————————————————————————————— OTHER  ——————————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	void Curtain::operator=(DeserializedJSON::DeserializedJSON& update_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		using namespace Message::Literal::JSON::Key;
		using namespace Message::Literal::MQTT;

		if(update_json.containsKey(HOME_ID) && update_json[HOME_ID] != _home_id)
		{
			if(_home_id != 0)
			{
				Global::mqtt_client.unsubscribe(HOME_MOVE);
				Global::mqtt_client.unsubscribe(HOME_UPDATE);
			}

			StaticString<0>::copy(update_json[HOME_ID], (char*)_home_id);
			HOME_MOVE.write(_home_id, 13);
			HOME_UPDATE.write(_home_id, 13);
			Global::mqtt_client.subscribe(HOME_MOVE);
			Global::mqtt_client.subscribe(HOME_UPDATE);
		}

		if(update_json.containsKey(ROOM_ID) && update_json[ROOM_ID] != _room_id)
		{
			if(_room_id != 0)
			{
				Global::mqtt_client.unsubscribe(ROOM_MOVE);
				Global::mqtt_client.unsubscribe(ROOM_UPDATE);
			}

			StaticString<0>::copy(update_json[ROOM_ID], (char*)_room_id);
			ROOM_MOVE.write(_room_id, 15);
			ROOM_UPDATE.write(_room_id, 15);
			Global::mqtt_client.subscribe(ROOM_MOVE);
			Global::mqtt_client.subscribe(ROOM_UPDATE);
		}

		// Validate hardware overriding values
		if(update_json.containsKey(LENGTH) && update_json[LENGTH] < _length)
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
