
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
#include "../Headers/Movement.hpp"
#include "../Headers/Transmission.hpp"


namespace Event
{
	using namespace Movement::CurtainStates;


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //

	Event::Event(JsonObject& event_object)
	{
		using namespace Transmission::Literal::JSON::Key;

		if(!event_object.containsKey(EVENT_ID))
		{
			String error_message = String("Key value \"") + EVENT + "\" is missing key: \"" + EVENT_ID + "\"";
			Exceptions::throw_HTTP_404(error_message.c_str());
		}
		if(!event_object.containsKey(EVENT_ID))
		{
			String error_message = String("Key value \"") + EVENT + "\" is missing key: \"" + EVENT_PERCENTAGE + "\"";
			Exceptions::throw_HTTP_404(error_message.c_str());
		}

		_id = event_object[EVENT_ID];

		// Set percentage based on functioning percentage since Events are consumed immediately.
		register uint8_t event_percentage = event_object[EVENT_PERCENTAGE];
		if(!event_percentage)
		{
			_percentage = 0;
		}
		else if(!Global::curtain.discrete_movement())
		{
			_percentage = 100;
		}
		else
		{
			_percentage = event_percentage;
		}
	}


	Event::Event(register uint32_t id, register uint8_t percentage)
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
		JsonObject event_object = (JsonObject)(*this);

		String json_string;
		serializeJson(event_object, json_string);

		return json_string;
	}


	// SUMMARY: Creates a JsonObject for partially JSONing the Curtain object.
	// DETAILS: Called when a Curtain object is attempted to be converted to a JsonObject. Adds object attributes to
	//  JsonObject and returns it.
	Event::operator JsonObject()
	{
		JsonObject event_object = JsonObject();

		event_object[Transmission::Literal::JSON::Key::EVENT_ID] = _id;
		event_object[Transmission::Literal::JSON::Key::EVENT_PERCENTAGE] = _percentage;

		return event_object;
	}


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

	uint32_t Event::id()
	{
		return _id;
	}


	uint8_t Event::percentage()
	{
		return _percentage;
	}


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
		CurtainState curtian_state = Movement::current_state();
		// parens not needed (precedence) but used to remove warnings
		return (curtian_state == CLOSED && _percentage == 100) || (curtian_state == OPEN && _percentage == 0);
	}


	// SUGAR: whether desired position is open/close/middle.
	CurtainState Event::state()
	{
		return Movement::state_of(_percentage);
	}
}  // end namespace Event
