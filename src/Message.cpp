
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
			const char ALL_CURTAINS[] = "SmartCurtain/all/status";
			const char MOVE_SUFFIX[] = "/move";
			const char STATUS_SUFFIX[] = "/status";
			const char UPDATE_SUFFIX[] = "/update";
			const char HUB_UPDATE_TOPIC[] = "SmartCurtain/hub/update";
		}  // end namespace MQTT


		namespace JSON
		{
			namespace Key
			{
				// CURTAIN
				// Structure
				const char CURTAIN_ID[] = "id";
				const char HOME_ID[] = "Home.id";
				const char ROOM_ID[] = "Room.id";

				// Hardware describing/overriding values
				const char DIRECTION[] = "direction";
				const char LENGTH[] = "length";
				const char PERCENTAGE[] = "percentage";

				// Movement describing/overriding values
				const char AUTO_CORRECT[] = "Auto Correct";

				// EVENT
				const char EVENT_IS_MOVING[] = "is moving";
				const char EVENT_PERCENTAGE[] = "percentage";
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
	}


	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //

	inline String convert_JsonObject_to_String(JsonObject& object)
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

	DeserializedJSON read_message(int message_size)
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

		return DeserializedJSON(message_buffer);
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
