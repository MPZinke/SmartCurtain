
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


#include "../Headers/Message.hpp"

#include "../Headers/Config.hpp"
#include "../Headers/Global.hpp"

#include "../Headers/C_String.hpp"
#include "../Headers/Curtain.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exceptions.hpp"


using namespace Exceptions;


namespace Message
{
	namespace Literal
	{
		namespace MQTT
		{
			const char CURTAIN_PATH_PREFIX[] = "SmartCurtain/-/-/";
			const char MOVE_SUFFIX[] = "/move";
			const char STATUS_SUFFIX[] = "/status";
			const char UPDATE_SUFFIX[] = "/update";
			const char HUB_UPDATE_TOPIC[] = "SmartCurtain/hub/update";
		}  // end namespace MQTT


		namespace JSON
		{
			namespace Key
			{
				const char QUERY_TYPE[] = "query type";

				const char CURTAIN[] = "curtain";
				const char CURTAIN_ID[] = "id";
				const char AUTO_CALIBRATE[] = "auto calibrate";
				const char AUTO_CORRECT[] = "auto correct";
				const char CURTAIN_PERCENTAGE[] = "percentage";
				const char CURTAIN_POSITION[] = "position";
				const char DIRECTION[] = "direction";
				const char DISCRETE_MOVEMENT[] = "discrete movement";
				const char LENGTH[] = "length";

				const char EVENT_IS_MOVING[] = "is moving";
				const char EVENT_PERCENTAGE[] = "percentage";

				const char HUB_IP[] = "Hub IP";
			}


			namespace Value
			{
				const char MOVE[] = "move";  // Query type value for move
				const char RESET[] = "reset";  // Query type value for reset
				const char STATUS[] = "status";  // Query type value for status
				const char UPDATE[] = "update";  // Query type value for update

				const ValueID VALUE_IDS[] =
				{
					{MOVE_ID, MOVE},
					{RESET_ID, RESET},
					{STATUS_ID, STATUS},
					{UPDATE_ID, UPDATE}
				};
			}
		}


		namespace Responses
		{
			const char INVALID[] = "{\"error\" : \"Package received does not match JSON format\"}";
			const char MOVING[] = "{\"success\":\"Moving to position\"}";
			const char VALID[] = "{\"success\":\"Valid JSON received\"}";
		}
	}


	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //

	String convert_JsonObject_to_String(JsonObject& object)
	{
		String json_string;
		serializeJson(object, json_string);

		return json_string;
	}


	String http_exception_json(uint16_t error_code, char error_message[])
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject error_object = json_document.to<JsonObject>();

		error_object["success"] = false;
		error_object["status code"] = error_code;
		error_object["message"] = error_message;

		return convert_JsonObject_to_String(error_object);
	}


	String status_json()
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject status_object = json_document.to<JsonObject>();

		Global::curtain.append_to(status_object);

		return convert_JsonObject_to_String(status_object);
	}


	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //

	String read_message(int message_size)
	{
		String message_buffer;
		for(register uint16_t x = 0; x < message_size; x++)
		{
			message_buffer += (char)Global::mqtt_client.read();
		}

		return message_buffer;
	}


	bool read_message(int message_size)
	/*
	SUMMARY: Reads the mqtt message from the client into the JSON document.
	PARAMS:  Takes the message size to be read.
	DETAILS: Checks whether the size is managable. If so the ...TODO
	RETURNS: Whether the message was successfully read into the JSON document.
	*/
	{
		String message_buffer;
		for(register uint16_t x = 0; x < message_size; x++)
		{
			message_buffer += (char)Global::mqtt_client.read();
		}

		if(deserializeJson(json_document, message_buffer))
		{
			new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Could not read request into json_buffer");
			return false;
		}

		return true;
	}


	bool unauthenticated(register uint32_t& read_count)
	{
		// Match "Authorization: " tag
		using namespace Literal::HTTP;
		for(register uint32_t header_index = 0; header_index < AUTH_HEADER_LENGTH && read_count < 0xFFFFFFFF
		  && Global::client.available(); read_count++)
		{
			char next = Global::client.read();
			// If starting a newline and another is found, data section reached; no auth header has been found
			if(next == '\r' && Global::client.available() && (next = Global::client.read()) == '\n')
			{
				return true;
			}

			// If the tag does not match...
			if(next != AUTHORIZATION_HEADER[header_index])
			{
				read_to_next_line(read_count);
				header_index = 0;
			}
			// Tag matches Auth tag
			else if(header_index == AUTH_HEADER_LENGTH-1)  // minus 1 for indexing
			{
				return false;
			}
			// Keep trying
			else
			{
				header_index++;
			}
		}

		return true;  // should never happen
	}


	/*
	SUMMARY: Determines whether the auth value is correct.
	DETAILS: Reads through each character from client matching the character to the state machine to match to a double 
	         carriage return-newline combo.
	RETURNS: Return whether the body of the client is found.
	*/
	bool unauthorized(register uint32_t& read_count)
	{
		using namespace Config;
		for(register uint32_t x = 0; x < HTTP::AUTH_VALUE_LENGTH && Global::client.available(); x++, read_count++)
		{
			char next = Global::client.read();
			if(next != HTTP::AUTHORIZATION_VALUE[x])
			{
				return true;
			}
		}

		return !(Global::client.available() && Global::client.peek() == '\r');
	}


	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //
	void update_hub()
	{
		String status_string = Message::status_json();
		Global::mqtt_client.beginMessage(Message::Literal::MQTT::HUB_UPDATE_TOPIC);
		Global::mqtt_client.print(status_string);
		Global::mqtt_client.endMessage();
	}
}
