
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


#include "Headers/Automation.hpp"


namespace Automation
{
	void automation_loop(void*)
	{
		while(true)
		{
			Movement::disable_motor();  // don't burn up the motor
		
			StaticJsonDocument<Global::JSON_BUFFER_SIZE> json_document = decode_json();
			const char* query_type = json_document[Transmission::Literals::JSON::Key::QUERY_TYPE];
	
			switch(Transmission::id_for_value(query_type))
			{
				case Transmission::id_for_value(Transmission::Literals::JSON::Value::STATUS):
					Transmission::send_status();
					break;
	
				// Reset curtain by moving it from alleged current position to close to actual current position.
				case Transmission::id_for_value(Transmission::Literals::JSON::Value::RESET):
					Movement::move_until_closed();
	
				// Move to position
				case Transmission::id_for_value(Transmission::Literals::JSON::Value::MOVE):
					Transmission::respond_with_json_and_stop(Transmission::Responses::VALID_RESPONSE);
					Curtain::Curtain curtain(json_document);  // setup data (things are getting real interesting...)
					curtain.move();
	
					// clean up and update curtain
					curtain.set_location();
					curtain.send_hub_serialized_info();
					break;
	
				case default:
					throw HTTP_Exception(404, "Unknown query type");
			}
	
			setjmp(Global::jump_buffer);
	
			delay(700);
		}
	}


	// RETURN: The JSON Document object.
	// THROWS: HTTPS_Exceptions
	StaticJsonDocument<Global::JSON_BUFFER_SIZE> decode_json()
	{
		Global::client = Transmission::wait_for_request();

		// bad message: retry later
		char* json_buffer = Transmission::read_transmission_data_into_buffer();
		if(!json_buffer)
		{
			Exceptions::throw_HTTP_204("Could not read transmission into json_buffer");
		}

		// Decode JSON
		StaticJsonDocument<Global::JSON_BUFFER_SIZE> json_document;
		bool successfully_deserialized = !deserializeJson(json_document, json_buffer);
		delete[] json_buffer;

		if(!successfully_deserialized)
		{
			Exceptions::throw_HTTP_400("Could not decode json");
		}

		return json_document;
	}
}