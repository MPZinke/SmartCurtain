
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
#include "../Headers/Message.hpp"


using namespace Exceptions;


namespace Processor
{
	void server_loop()
	{
		while(true)
		{
			StaticJsonDocument<JSON_BUFFER_SIZE> json_document = Message::decode_json();
			if(!Global::exception)
			{
				// If curtain information, update Global::curtain information
				if(json_document.containsKey(Message::Literal::JSON::Key::CURTAIN))
				{
					Global::curtain.update(json_document);
				}

				// Call action for QUERY_TYPE
				switch(Message::id_for_query_type(json_document))
				{
					// Update information about curtain
					case Message::Literal::JSON::Value::UPDATE_ID:
					{
Serial.println(String("Line: ") + __LINE__);
						case_update(json_document);
						// Fall through to STATUS_ID
					}

					// Return information about Curtain
					case Message::Literal::JSON::Value::STATUS_ID:
					{
Serial.println(String("Line: ") + __LINE__);
						case_status();
						break;
					}

					// Move to position
					case Message::Literal::JSON::Value::MOVE_ID:
					{
Serial.println(String("Line: ") + __LINE__);
						case_move(json_document);
						break;
					}

					default:
					{
Serial.println(String("Line: ") + __LINE__);
						case_default(json_document);
					}
				}
			}
Serial.println(String("Line: ") + __LINE__);

			if(Global::exception)
			{
Serial.println(String("Line: ") + __LINE__);
				Global::exception->send();
			}
		}
	}


	// ——————————————————————————————————————————————————— CASES  ——————————————————————————————————————————————————— //

	void case_default(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		String message = String("Unknown ") + Message::Literal::JSON::Key::QUERY_TYPE + " '"
		  + (const char*)json_document[Message::Literal::JSON::Key::QUERY_TYPE] + "'";
		new NOT_FOUND_404_Exception(__LINE__, __FILE__, message);
	}


	void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		if(!Global::event.is_finished())
		{
			return (void)new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Event already exists");
		}

		if(!json_document.containsKey(Message::Literal::JSON::Key::EVENT))
		{
			String error_message = String("Missing key: '") + Message::Literal::JSON::Key::EVENT + "' for QUERY_TYPE: '"
			  + Message::Literal::JSON::Value::MOVE + "'";
			return (void)new NOT_FOUND_404_Exception(__LINE__, __FILE__, error_message);
		}

		JsonObject event_object = json_document[Message::Literal::JSON::Key::EVENT];
		Global::event = Event::Event(event_object);

		if(!Global::exception)
		{
			Message::respond_with_json_and_stop(Message::Literal::Responses::MOVING);
		}
	}


	void case_status()
	{
		String status_string = Message::status_json();
		Message::respond_with_json_and_stop(status_string);
	}


	void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		using namespace Message::Literal::JSON;

		if(!json_document.containsKey(Key::HUB_IP))
		{
			uint8_t octets[4];
			C_String::IP_address_octets(json_document[Key::HUB_IP], octets);
			Global::client_IP = IPAddress(octets[0], octets[1], octets[2], octets[3]);
		}
	}
}
