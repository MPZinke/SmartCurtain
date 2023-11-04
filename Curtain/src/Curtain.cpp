

#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/MQTT.hpp"
#include "../Headers/StaticString.hpp"


// Structure
#define CURTAIN_ID "CURTAIN_ID"
#define HOME_ID "HOME_ID"
#define ROOM_ID "ROOM_ID"
// Hardware describing/overriding values
#define LENGTH "LENGTH"
#define PERCENTAGE "PERCENTAGE"
// Movement describing/overriding values
#define AUTO_CORRECT "AUTO_CORRECT"
// Non-Overridable
#define IS_MOVING "IS_MOVING"


namespace Curtain
{
	using namespace Movement::CurtainStates;


	bool validate(DeserializedJSON::DeserializedJSON& curtain_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		// Validate structure
		if(!curtain_json.containsKey(CURTAIN_ID) || !curtain_json[CURTAIN_ID].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Curtain object requires key '"CURTAIN_ID"' of type 'int'");
			return false;
		}

		else if(curtain_json.containsKey(HOME_ID) && !curtain_json[HOME_ID].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Curtain object requires key '"HOME_ID"' of type 'int'");
			return false;
		}

		else if(curtain_json.containsKey(ROOM_ID) && !curtain_json[ROOM_ID].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Curtain object requires key '"ROOM_ID"' of type 'int'");
			return false;
		}

		// Validate hardware overriding values
		else if(curtain_json.containsKey(LENGTH) && !curtain_json[LENGTH].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Curtain object requires key '"LENGTH"' of type 'int'");
			return false;
		}

		// Validate movement overriding values
		else if(curtain_json.containsKey(AUTO_CORRECT) && !curtain_json[AUTO_CORRECT].is<bool>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Curtain object requires key '"AUTO_CORRECT"' of type 'bool'");
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

		curtain_object[CURTAIN_ID] = (const char[])_id;
		curtain_object[ROOM_ID] = (const char[])_room_id;
		curtain_object[HOME_ID] = (const char[])_home_id;
		curtain_object[IS_MOVING] = _is_moving;
		curtain_object[LENGTH] = _length;
		curtain_object[PERCENTAGE] = percentage();
		curtain_object[AUTO_CORRECT] = _auto_correct;

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
		if(_room_id != BLANK_OBJECT_ID)
		{
			Global::mqtt_client.unsubscribe(MQTT::ROOM_MOVE);
			Global::mqtt_client.unsubscribe(MQTT::ROOM_UPDATE);
		}

		MQTT::ROOM_MOVE.overwrite(_room_id, new_room_id);
		MQTT::ROOM_UPDATE.overwrite(_room_id, new_room_id);
		Global::mqtt_client.subscribe(MQTT::ROOM_MOVE);
		Global::mqtt_client.subscribe(MQTT::ROOM_UPDATE);

		_room_id = new_room_id;
	}


	void Curtain::home_id(const char* new_home_id)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(_home_id != BLANK_OBJECT_ID)
		{
			Global::mqtt_client.unsubscribe(MQTT::HOME_MOVE);
			Global::mqtt_client.unsubscribe(MQTT::HOME_UPDATE);
		}

		MQTT::HOME_MOVE.overwrite(_home_id, new_home_id);
		MQTT::HOME_UPDATE.overwrite(_home_id, new_home_id);
		Global::mqtt_client.subscribe(MQTT::HOME_MOVE);
		Global::mqtt_client.subscribe(MQTT::HOME_UPDATE);

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


	void Curtain::length(uint32_t new_length)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(new_length < Config::Hardware::MAX_LENGTH)
		{
			_length = new_length;
		}
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
		using namespace MQTT;

		if(update_json.containsKey(HOME_ID) && update_json[HOME_ID] != _home_id)
		{
			home_id(update_json[HOME_ID]);
		}

		if(update_json.containsKey(ROOM_ID) && update_json[ROOM_ID] != _room_id)
		{
			room_id(update_json[ROOM_ID]);
		}

		// Validate hardware overriding values
		if(update_json.containsKey(LENGTH) && update_json[LENGTH] < _length)
		{
			length(update_json[LENGTH]);
		}

		// Validate movement overriding values
		if(update_json.containsKey(AUTO_CORRECT))
		{
			auto_correct(update_json[AUTO_CORRECT]);
		}
	}
}
