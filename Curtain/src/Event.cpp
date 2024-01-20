
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.19                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Event objects are supposed to be relatively dumb—they are supposed to know where they want to go      *
*    generally speaking and that is it. How an event acheives its movement means nothing to it.                        *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Event.hpp"


#include "../Headers/Global.hpp"

#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/MQTT.hpp"
#include "../Headers/StaticString.hpp"


namespace Event
{
	using namespace Curtain::CurtainStates;


	namespace Errors
	{
		namespace Literals
		{
			const char ERROR_START[] = "Event object requires key '";
			const char INT_ERROR_END[] = "' of type 'int'";
		}

		namespace Sizes
		{
			using namespace ::Event::Keys;
			using namespace ::Event::Errors::Literals;

			const uint16_t PERCENTAGE_KEY = sizeof(ERROR_START) +sizeof(PERCENTAGE) + sizeof(INT_ERROR_END) - 2;
		}

		using namespace Literals;

		StaticString<Sizes::PERCENTAGE_KEY-1> PERCENTAGE(ERROR_START, ::Event::Keys::PERCENTAGE, INT_ERROR_END);
	}


	bool validate(DeserializedJSON::DeserializedJSON& event_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		// Validate structure
		if(event_json.containsKey(Keys::PERCENTAGE) && !event_json[Keys::PERCENTAGE].is<int>())
		{
			const char error_start[] = "Event object requires key '";
			const char error_end[] = "' of type 'int'";
			const uint16_t error_size = sizeof(error_start) + sizeof(Keys::PERCENTAGE) + sizeof(error_end) - 3;
			StaticString<error_size> error(error_start, Keys::PERCENTAGE, error_end);

			new Exception::Exception(__LINE__, __FILE__, error);
			return false;
		}

		return true;
	}

	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //


	Event::Event(DeserializedJSON::DeserializedJSON& event_json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_percentage = event_json[Keys::PERCENTAGE];
	}


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

	uint8_t Event::percentage()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _percentage;
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //

	CurtainState Event::direction()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(_percentage < Global::curtain.percentage())
		{
			return CLOSE;
		}

		return OPEN;
	}


	bool Event::event_moves_to_an_end()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _percentage == 0 || _percentage == 100;
	}


	uint32_t Event::steps()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		uint8_t percentage_delta = Global::curtain.percentage() - _percentage;
		if(percentage_delta < 0)
		{
			percentage_delta *= -1;
		}

		return percentage_delta * Global::curtain._length / 100;
	}
}  // end namespace Event


template class StaticString<Event::Errors::Sizes::PERCENTAGE_KEY-1>;
