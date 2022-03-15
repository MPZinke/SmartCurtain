
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.10                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Automation.hpp"

#include "../Headers/Global.hpp"
#include "../Headers/C_String.hpp"
#include "../Headers/Curtain.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exceptions.hpp"
#include "../Headers/Movement.hpp"
#include "../Headers/Request.hpp"


namespace Automation
{
	void automation_loop(void*)
	{
		while(true)
		{
			Hardware::disable_motor();  // don't burn up the motor
		
			StaticJsonDocument<JSON_BUFFER_SIZE> json_document = decode_json();

			// If curtain information, update Global::curtain information
			if(json_document.containsKey(Request::Literal::JSON::Key::CURTAIN))
			{
				Global::curtain.update(json_document);
			}

			// Call action for QUERY_TYPE
			switch(Request::id_for_value(json_document[Request::Literal::JSON::Key::QUERY_TYPE]))
			{
				// Update information about curtain
				case Request::Literal::JSON::Value::UPDATE_ID:
				{
					case_update(json_document);
					// Fall through to STATUS_ID
				}

				// Return information about Curtain
				case Request::Literal::JSON::Value::STATUS_ID:
				{
					Request::send_status_and_stop_client();
					break;
				}
	
				// Reset curtain by moving it from alleged current position to close to actual current position.
				case Request::Literal::JSON::Value::RESET_ID:
				{
					Movement::EndstopGuarded::move_and_reset();
					break;
				}

				// Move to position
				case Request::Literal::JSON::Value::MOVE_ID:
				{
					case_move(json_document);
					break;
				}

				default:
				{
					Exceptions::throw_HTTP_404("Unknown query type");
				}
			}
	
			setjmp(Global::jump_buffer);
	
			delay(700);
		}
	}


	// RETURN: The JSON Document object.
	// THROWS: HTTPS_Exceptions
	StaticJsonDocument<JSON_BUFFER_SIZE> decode_json()
	{
		Global::client = Request::wait_for_request();

		// bad message: retry later
		String json_buffer = Request::read_request_data_into_buffer();
		if(!json_buffer.length())
		{
			Exceptions::throw_HTTP_204("Could not read Request into json_buffer");
		}

		// Decode JSON
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		if(!deserializeJson(json_document, json_buffer))
		{
			Exceptions::throw_HTTP_400("Could not decode json");
		}

		return json_document;
	}


	// ——————————————————————————————————————————————————— CASES  ——————————————————————————————————————————————————— //

	void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		if(!json_document.containsKey(Request::Literal::JSON::Key::EVENT))
		{
			String error_message = String("Missing key: \"") + Request::Literal::JSON::Key::EVENT
			  + "\" for QUERY_TYPE: \"" + Request::Literal::JSON::Value::MOVE + "\"";
			Exceptions::throw_HTTP_404(error_message.c_str());
		}

		Request::respond_with_json_and_stop(Request::Literal::Responses::VALID);
		JsonObject event_object = json_document[Request::Literal::JSON::Key::EVENT];
		Event::Event event(event_object);

		Movement::activate(event);
	}


	void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		using namespace Request::Literal::JSON;

		if(json_document.containsKey(Key::HUB_IP))
		{
			C_String::copy(json_document[Key::HUB_IP], Config::Network::HUB_HOST_STR);
		}
	}
}
