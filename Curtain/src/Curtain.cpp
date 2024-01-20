

#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/MQTT.hpp"
#include "../Headers/Movement.hpp"
#include "../Headers/StaticString.hpp"


namespace Curtain
{
	using namespace Movement::CurtainStates;


	namespace Keys
	{
		// Structure
		const char CURTAIN_NAME[] = "name";
		const char ROOM_NAME[] = "room";
		// Hardware describing/overriding values
		const char LENGTH[] = "length";
		const char PERCENTAGE[] = "percentage";
		// Movement describing/overriding values
		const char AUTO_CORRECT[] = "auto correct";
		// Non-Overridable
		const char IS_MOVING[] = "is moving";
	}


	namespace Errors
	{
		namespace Literals
		{
			const char ERROR_START[] = "Curtain object requires key '";
			const char BOOL_ERROR_END[] = "' of type 'bool'";
			const char INT_ERROR_END[] = "' of type 'int'";
		}

		namespace Sizes
		{
			using namespace ::Curtain::Keys;
			using namespace ::Curtain::Errors::Literals;

			const uint16_t AUTO_CORRECT = sizeof(ERROR_START) + sizeof(AUTO_CORRECT) + sizeof(BOOL_ERROR_END) - 2;
			const uint16_t LENGTH = sizeof(ERROR_START) + sizeof(LENGTH) + sizeof(INT_ERROR_END) - 2;
		}

		using namespace Literals;

		StaticString<Sizes::AUTO_CORRECT-1> AUTO_CORRECT(ERROR_START, ::Curtain::Keys::AUTO_CORRECT, BOOL_ERROR_END);
		StaticString<Sizes::LENGTH-1> LENGTH(ERROR_START, ::Curtain::Keys::LENGTH, INT_ERROR_END);
	}


	bool validate(DeserializedJSON::DeserializedJSON& curtain_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		// Validate movement overriding values
		if(curtain_json.containsKey(Keys::AUTO_CORRECT) && !curtain_json[Keys::AUTO_CORRECT].is<bool>())
		{
			new Exception::Exception(__LINE__, __FILE__, (const char*)Errors::AUTO_CORRECT);
			return false;
		}

		// Validate hardware overriding values
		else if(curtain_json.containsKey(Keys::LENGTH) && !curtain_json[Keys::LENGTH].is<int>())
		{
			new Exception::Exception(__LINE__, __FILE__, (const char*)Errors::LENGTH);
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

		curtain_object[Keys::CURTAIN_NAME] = (const char*)_name;
		curtain_object[Keys::ROOM_NAME] = (const char*)_room;
		curtain_object[Keys::IS_MOVING] = _is_moving;
		curtain_object[Keys::LENGTH] = _length;
		curtain_object[Keys::PERCENTAGE] = percentage();
		curtain_object[Keys::AUTO_CORRECT] = _auto_correct;

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
		if(update_json.containsKey(Keys::LENGTH) && update_json[Keys::LENGTH] < _length)
		{
			length(update_json[Keys::LENGTH]);
		}

		// Validate movement overriding values
		if(update_json.containsKey(Keys::AUTO_CORRECT))
		{
			auto_correct(update_json[Keys::AUTO_CORRECT]);
		}
	}
}


template class StaticString<Curtain::Errors::Sizes::AUTO_CORRECT-1>;
template class StaticString<Curtain::Errors::Sizes::LENGTH-1>;
