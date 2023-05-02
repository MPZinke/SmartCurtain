
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


#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exception.hpp"


using namespace Exception;


namespace Message
{
	namespace Literal
	{
		namespace MQTT
		{
			const char PATH_PREFIX[] = "SmartCurtain/";
			const char CURTAIN_PATH_PREFIX[] = "SmartCurtain/-/-/";
			const char ALL_CURTAINS[] = "SmartCurtain/all/status";
			const char MOVE_SUFFIX[] = "/move";
			const char STATUS_SUFFIX[] = "/status";
			const char UPDATE_SUFFIX[] = "/update";
			const char HUB_UPDATE_TOPIC[] = "SmartCurtain/hub/update";
			const char HUB_ERROR_TOPIC[] = "SmartCurtain/hub/error";
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
				const char LENGTH[] = "length";
				const char PERCENTAGE[] = "percentage";

				// Movement describing/overriding values
				const char AUTO_CORRECT[] = "Auto Correct";

				// Non-Overridable
				const char IS_MOVING[] = "is_moving";
			}
		}
	}


	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //

	String convert_JsonObject_to_String(JsonObject& object)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		String json_string;
		serializeJson(object, json_string);

		return json_string;
	}


	String http_exception_json(uint16_t error_code, char error_message[])
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject error_object = json_document.to<JsonObject>();

		error_object["success"] = false;
		error_object["status code"] = error_code;
		error_object["message"] = error_message;

		return convert_JsonObject_to_String(error_object);
	}


	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //

	DeserializedJSON::DeserializedJSON read_message(int message_size)
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

		return DeserializedJSON::DeserializedJSON(message_buffer);
	}


	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //

	void update_hub()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		String message;
		const char* topic;
		if(Global::exception != NULL)
		{
			topic = Message::Literal::MQTT::HUB_ERROR_TOPIC;
			message = (String)(*Global::exception);
			delete Global::exception;
		}
		else
		{
			topic = Message::Literal::MQTT::HUB_UPDATE_TOPIC;
			message = (String)(Global::curtain);
		}

		Global::mqtt_client.beginMessage(topic);
		Global::mqtt_client.print(message);
		Global::mqtt_client.endMessage();
	}
}
