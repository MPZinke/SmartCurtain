
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
#include "../Headers/Exceptions.hpp"
#include "../Headers/Movement.hpp"
#include "../Headers/Message.hpp"


using namespace Exceptions;


namespace Event
{
	using namespace Movement::CurtainStates;


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //

	Event::Event(String& event_json)
	{
		if(deserializeJson(Global::json_document, event_json))
		{
			new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Could not read Event into JSON buffer");
			return;
		}

		if(!json_document.containsKey("percentage"))
		{
			new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Event JSON does not contain key 'percentage'");
			return;
		}

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


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

	bool Event::is_activated()
	{
		return _is_activated;
	}


	bool Event::is_moving()
	{
		return _is_moving;
	}


	uint8_t Event::percentage()
	{
		return _percentage;
	}

	// ——————————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————————— //

	// SUMMARY: Creates a String JSON for the Event.
	// DETAILS: Called when a Curtain object is attempted to be converted to a char*. Converts object to a JsonObject.
	//  Mallocs char* array for c_string. Serializes data to c_string.
	Event::operator String()
	{
		using namespace Message::Literal;  // not entire namespace to help show where the below values are from
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject event_object = json_document.to<JsonObject>();

		event_object[JSON::Key::EVENT_IS_MOVING] = _is_moving;
		event_object[JSON::Key::EVENT_PERCENTAGE] = _percentage;

		return Message::convert_JsonObject_to_String(event_object);
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

	void Event::is_activated(bool new_is_activated)
	{
		_is_activated = new_is_activated;
	}


	void Event::is_moving(bool new_is_moving)
	{
		_is_moving = new_is_moving;
	}
}  // end namespace Event
