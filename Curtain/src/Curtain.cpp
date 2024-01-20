

#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/MQTT.hpp"
#include "../Headers/Movement.hpp"
#include "../Headers/StaticString.hpp"


// Structure
#define CURTAIN_NAME_KEY "name"
#define ROOM_NAME_KEY "room"
// Hardware describing/overriding values
#define LENGTH_KEY "length"
#define PERCENTAGE_KEY "percentage"
// Movement describing/overriding values
#define AUTO_CORRECT_KEY "auto_correct"
// Non-Overridable
#define IS_MOVING_KEY "is_moving"


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
		// Validate hardware overriding values
		if(curtain_json.containsKey(LENGTH_KEY) && !curtain_json[LENGTH_KEY].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Curtain object requires key '"LENGTH_KEY"' of type 'int'");
			return false;
		}

		// Validate movement overriding values
		else if(curtain_json.containsKey(AUTO_CORRECT_KEY) && !curtain_json[AUTO_CORRECT_KEY].is<bool>())
		{
			new Exception::Exception(__LINE__, __FILE__, "Curtain object requires key '"AUTO_CORRECT_KEY"' of type 'bool'");
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
	: _name{StaticString<sizeof(Config::Curtain::NAME)>(Config::Curtain::NAME)},
	  _room{StaticString<sizeof(Config::Curtain::ROOM)>(Config::Curtain::ROOM)}
	{}


	// —————————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————————— GETTERS::STRUCTURE ————————————————————————————————————————————— //

	StaticString<sizeof(Config::Curtain::NAME)> Curtain::name() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return StaticString<sizeof(Config::Curtain::NAME)>(_name);
	}


	StaticString<sizeof(Config::Curtain::ROOM)> Curtain::room() const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return StaticString<sizeof(Config::Curtain::ROOM)>(_room);
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

		curtain_object[CURTAIN_NAME_KEY] = (const char*)_name;
		curtain_object[ROOM_NAME_KEY] = (const char*)_room;
		curtain_object[IS_MOVING_KEY] = _is_moving;
		curtain_object[LENGTH_KEY] = _length;
		curtain_object[PERCENTAGE_KEY] = percentage();
		curtain_object[AUTO_CORRECT_KEY] = _auto_correct;

		return StaticString<JSON_BUFFER_SIZE>(curtain_object);
	}


	// —————————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

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

		// Validate hardware overriding values
		if(update_json.containsKey(LENGTH_KEY) && update_json[LENGTH_KEY] < _length)
		{
			length(update_json[LENGTH_KEY]);
		}

		// Validate movement overriding values
		if(update_json.containsKey(AUTO_CORRECT_KEY))
		{
			auto_correct(update_json[AUTO_CORRECT_KEY]);
		}
	}
}
