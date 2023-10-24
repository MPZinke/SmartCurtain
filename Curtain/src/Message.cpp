
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
			const char ALL_CURTAINS_MOVE[] = "SmartCurtain/all/move";
			const char ALL_CURTAINS_STATUS[] = "SmartCurtain/all/status";
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
	SUMMARY: Syntactic Sugar to convert a JsonObject to a String.
	PARAMS:  Takes the JsonObject to convert
	RETURNS: The String converted from the JsonObject
	*/
	{
		String json_string;
		serializeJson(object, json_string);

		return json_string;
	}


	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //

	DeserializedJSON::DeserializedJSON read_message(int message_size)
	/*
	SUMMARY: Reads the mqtt message from the client into the DeserializedJSON object.
	PARAMS:  Takes the message size to be read.
	DETAILS: Reads the message into a string buffer. The string buffer is then deserialized in the DeserializedJSON
	         object.
	RETURNS: Returns the DeserializedJSON that holds the JSON and whether the message is a valid JSON.
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
	SUMMARY: Updates the hub with either the Curtain JSON or and exception if one is listed.
	PARAMS:  GLOBAL::exception && GLOBAL::curtain.
	DETAILS: Checks whether an exception is listed. If so, it converts that to a JSON and publishes to the hub's error
	         topic. Otherwise it converts the curtain to a JSON and posts that to the hub's `.../update` topic.
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
