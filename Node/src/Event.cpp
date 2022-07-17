
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

#include "../Headers/Curtain.hpp"
#include "../Headers/Exceptions.hpp"
#include "../Headers/Movement.hpp"
#include "../Headers/Request.hpp"


using namespace Exceptions;


namespace Event
{
	using namespace Movement::CurtainStates;


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //

	Event::Event(JsonObject& event_object)
	{
		using namespace Request::Literal::JSON::Key;

		if(!event_object.containsKey(EVENT_ID))
		{
			String error_message = String("Key value \"") + EVENT + "\" is missing key: \"" + EVENT_ID + "\"";
			new BAD_REQUEST_400_Exception(__LINE__, __FILE__, error_message);
			return;
		}

		_id = event_object[EVENT_ID];

		// Set percentage based on functioning percentage since Events are consumed immediately.
		register uint8_t event_percentage = event_object[EVENT_PERCENTAGE];
		if(!event_percentage)
		{
			_percentage = 0;
		}
		else if(!Global::curtain.discrete_movement() || event_percentage > 100)
		{
			_percentage = 100;
		}
		else
		{
			_percentage = event_percentage;
		}
	}


	Event::Event(register uint32_t id, register uint8_t percentage, bool is_activated/*=false*/)
	{
		_id = id;

		// Set percentage based on functioning percentage since Events are consumed immediately.
		if(!percentage)
		{
			_percentage = 0;
		}
		else if(!Global::curtain.discrete_movement())
		{
			_percentage = 100;
		}
		else
		{
			_percentage = percentage;
		}
	}


	// FREE ME WHEN DONE
	// SUMMARY: Creates a malloced char array the size of the serialized json and writes it.
	// DETAILS: Called when a Curtain object is attempted to be converted to a char*. Converts object to a JsonObject.
	//  Mallocs char* array for c_string. Serializes data to c_string.
	Event::operator String()
	{
		using namespace Request::Literal;  // not entire namespace to help show where the below values are from
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject event_object = json_document.to<JsonObject>();

		event_object[JSON::Key::EVENT_ID] = _id;
		event_object[JSON::Key::EVENT_PERCENTAGE] = _percentage;

		return Request::convert_JsonObject_to_String(event_object);
	}


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

	uint32_t Event::id()
	{
		return _id;
	}


	bool Event::is_activated()
	{
		return _is_activated;
	}


	uint8_t Event::percentage()
	{
		return _percentage;
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //

	CurtainState Event::direction()
	{
		if(_percentage < Global::curtain.percentage()) return CLOSE;
		else if(Global::curtain.percentage() < _percentage) return OPEN;
		return MIDDLE;
	}


	bool Event::event_moves_to_an_end()
	{
		return _percentage == 0 || _percentage == 100;
	}


	// Determines whether the curtain moves all the way across the rod (open to close) for desired position.
	// Get the state of the curtain based of GPIO.  Compares with the state of the desired position.
	// Returns true if curtain moves all the way across rod, false otherwise.
	bool Event::moves_full_span()
	{
		CurtainState curtian_state = Global::curtain.state();
		// parens not needed (precedence) but used to remove warnings
		return (curtian_state == CLOSED && _percentage == 100) || (curtian_state == OPEN && _percentage == 0);
	}


	// SUGAR: whether desired position is open/close/middle.
	CurtainState Event::state()
	{
		return Movement::state_of(_percentage);
	}


	// ——————————————————————————————————————————————————— SETTER ——————————————————————————————————————————————————— //

	void Event::is_activated(bool _is_activated)
	{
		this->_is_activated = _is_activated;
	}
}  // end namespace Event
