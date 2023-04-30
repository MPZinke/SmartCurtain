
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
#include "../Headers/Exception.hpp"
#include "../Headers/Message.hpp"


using namespace Exception;


namespace Event
{
	using namespace Curtain::CurtainStates;


	bool validate(DeserializedJSON& event_json)
	{
		using namespace Message::Literal::JSON::Key;

		// Validate structure
		if(event_json.containsKey(PERCENTAGE) && !event_json[PERCENTAGE].is<int>())
		{
			new Exception(__LINE__, __FILE__, invalid_key_message(PERCENTAGE, "int"));
			return false;
		}

		return true;
	}

	// ————————————————————————————————————————— CONSTRUCTORS && CONVERTERS ————————————————————————————————————————— //


	Event::Event(DeserializedJSON& event_json)
	{
		using namespace Message::Literal::JSON::Key;

		_percentage = event_json[PERCENTAGE];
	}


	// ——————————————————————————————————————————————————— GETTER ——————————————————————————————————————————————————— //

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
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject event_object = json_document.to<JsonObject>();

		event_object[Message::Literal::JSON::Key::PERCENTAGE] = _percentage;

		return Message::convert_JsonObject_to_String(event_object);
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //

	CurtainState Event::direction()
	{
		if(_percentage < Global::curtain.percentage()) return CLOSE;
		return OPEN;
	}


	bool Event::event_moves_to_an_end()
	{
		return _percentage == 0 || _percentage == 100;
	}
}  // end namespace Event
