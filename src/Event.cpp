
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.19                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Event.hpp"

#include "../Headers/Movement.hpp"
#include "../Headers/Transmission.hpp"


namespace Event
{
	using namespace Movement::CurtainStates;


	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //

	Event::Event(JsonObject& event_object)
	{
		if(!event_object.containsKey(Transmission::Literal::JSON::Key::EVENT_ID))
		{
			String error_message = String("Key value \"") + Transmission::Literal::JSON::Key::EVENT
			  + "\" is missing key: \"" + Transmission::Literal::JSON::Key::EVENT_ID + "\"";
			Exceptions::throw_HTTP_404(error_message.c_str());
		}

		if(!event_object.containsKey(Transmission::Literal::JSON::Key::EVENT_ID))
		{
			String error_message = String("Key value \"") + Transmission::Literal::JSON::Key::EVENT
			  + "\" is missing key: \"" + Transmission::Literal::JSON::Key::EVENT_PERCENTAGE + "\"";
			Exceptions::throw_HTTP_404(error_message.c_str());
		}

		_id = event_object[Transmission::Literal::JSON::Key::EVENT_ID];
		_percentage = event_object[Transmission::Literal::JSON::Key::EVENT_PERCENTAGE];
	}


	Event::Event(uint32_t id, uint8_t percentage)
	{
		_id = id;
		_percentage = percentage;
	}


	// FREE ME WHEN DONE
	// SUMMARY: Creates a malloced char array the size of the serialized json and writes it.
	// DETAILS: Called when a Curtain object is attempted to be converted to a char*. Converts object to a JsonObject.
	//  Mallocs char* array for c_string. Serializes data to c_string.
	Event::operator char*()
	{
		JsonObject event_object = (JsonObject)(*this);

		size_t c_string_size = measureJson(event_object) + 1;
		char* json_c_string = (char*)malloc(c_string_size);
		serializeJson(event_object, json_c_string, c_string_size);

		return json_c_string;
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
		return _percentage == 0 || _percentage == Global::curtain.length();
	}


	// Determines whether the curtain moves all the way across the rod (open to close) for desired position.
	// Get the state of the curtain based of GPIO.  Compares with the state of the desired position.
	// Returns true if curtain moves all the way across rod, false otherwise.
	bool Event::moves_full_span()
	{
		CurtainState curtian_state = Movement::current_state();
		CurtainState desired_state = Movement::state_of(_percentage);
		// parens not needed (precedence) but used to remove warnings
		return (curtian_state == CLOSED && desired_state == OPEN) || (curtian_state == OPEN && desired_state == CLOSED);
	}


	// SUGAR: whether desired position is open/close/middle.
	CurtainState Event::state()
	{
		return Movement::state_of(_percentage);
	}
}  // end namespace Event
