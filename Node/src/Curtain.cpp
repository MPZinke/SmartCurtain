

#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/Message.hpp"
#include "../Headers/Global.hpp"


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

	uint16_t Curtain::id()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _id;
	}


	uint16_t Curtain::room_id()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _room_id;
	}


	uint16_t Curtain::home_id()
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

	bool Curtain::auto_correct()
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

	uint32_t Curtain::length()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _length;
	}


	bool Curtain::direction()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _direction;
	}


	bool Curtain::is_moving()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _is_moving;
	}


	uint32_t Curtain::percentage()
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

	Curtain::operator String()
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

		return Message::convert_JsonObject_to_String(curtain_object);
	}


	// —————————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————————— SETTERS::STRUCTURE ————————————————————————————————————————————— //

	void Curtain::room_id(uint16_t new_room_id)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_room_id = new_room_id;
	}


	void Curtain::home_id(uint16_t new_home_id)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_home_id = new_home_id;
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
				Global::mqtt_client.unsubscribe(String(PATH_PREFIX)+_home_id+MOVE_SUFFIX);
				Global::mqtt_client.unsubscribe(String(PATH_PREFIX)+_home_id+UPDATE_SUFFIX);
			}

			_home_id = update_json[HOME_ID];
			Global::mqtt_client.subscribe(String(PATH_PREFIX)+_home_id+MOVE_SUFFIX);
			Global::mqtt_client.subscribe(String(PATH_PREFIX)+_home_id+UPDATE_SUFFIX);
		}

		if(update_json.containsKey(ROOM_ID) && update_json[ROOM_ID] != _room_id)
		{
			if(_room_id != 0)
			{
				Global::mqtt_client.unsubscribe(String(PATH_PREFIX)+"-/"+_room_id+MOVE_SUFFIX);
				Global::mqtt_client.unsubscribe(String(PATH_PREFIX)+"-/"+_room_id+UPDATE_SUFFIX);
			}

			_room_id = update_json[ROOM_ID];
			Global::mqtt_client.subscribe(String(PATH_PREFIX)+"-/"+_room_id+MOVE_SUFFIX);
			Global::mqtt_client.subscribe(String(PATH_PREFIX)+"-/"+_room_id+UPDATE_SUFFIX);
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
