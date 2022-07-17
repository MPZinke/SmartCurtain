
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2022.07.12                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Processor.hpp"
#include "../Headers/Global.hpp"

#include "../Headers/C_String.hpp"
#include "../Headers/Curtain.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exceptions.hpp"
#include "../Headers/Request.hpp"


using namespace Exceptions;


namespace Processor
{
	void server_loop()
	{
		while(true)
		{
			StaticJsonDocument<JSON_BUFFER_SIZE> json_document = Request::decode_json();
			if(!Global::exception)
			{
				// If curtain information, update Global::curtain information
				if(json_document.containsKey(Request::Literal::JSON::Key::CURTAIN))
				{
					Global::curtain.update(json_document);
				}

				// Call action for QUERY_TYPE
				switch(Request::id_for_query_type(json_document))
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

					// Move to position
					case Request::Literal::JSON::Value::MOVE_ID:
					{
						case_move(json_document);
						break;
					}

					default:
					{
						case_default(json_document);
					}
				}
			}

			if(Global::exception)
			{
				Global::exception->send();
			}
		}
	}


	// ——————————————————————————————————————————————————— CASES  ——————————————————————————————————————————————————— //

	void case_default(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		String message = String("Unknown ") + Request::Literal::JSON::Key::QUERY_TYPE + " '"
		  + (const char*)json_document[Request::Literal::JSON::Key::QUERY_TYPE] + "'";
		new NOT_FOUND_404_Exception(__LINE__, __FILE__, message);
	}

	void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		if(!json_document.containsKey(Request::Literal::JSON::Key::EVENT))
		{
			String error_message = String("Missing key: '") + Request::Literal::JSON::Key::EVENT + "' for QUERY_TYPE: '"
			  + Request::Literal::JSON::Value::MOVE + "'";
			new NOT_FOUND_404_Exception(__LINE__, __FILE__, error_message);
			return;
		}

		// TODO: If !event.is_activated(): Exception
		Request::respond_with_json_and_stop(Request::Literal::Responses::VALID);
		JsonObject event_object = json_document[Request::Literal::JSON::Key::EVENT];
		Global::event = Event::Event(event_object);

		Request::deactivate_curtain();
	}


	void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		using namespace Request::Literal::JSON;

		if(!json_document.containsKey(Key::HUB_IP))
		{
			uint8_t octets[4];
			C_String::IP_address_octets(json_document[Key::HUB_IP], octets);
			Global::client_IP = IPAddress(octets[0], octets[1], octets[2], octets[3]);
		}
	}
}
