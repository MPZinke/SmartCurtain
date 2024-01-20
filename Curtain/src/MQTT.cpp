
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


#include "../Headers/MQTT.hpp"

#include "../Headers/Config.hpp"
#include "../Headers/Global.hpp"


#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/StaticString.hpp"


using namespace Exception;


namespace MQTT
{
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
		StaticString<JSON_BUFFER_SIZE> message_buffer;
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
		if(Global::exception != NULL)
		{
			StaticString<JSON_BUFFER_SIZE> message = (StaticString<JSON_BUFFER_SIZE>)(*Global::exception);
			delete Global::exception;

			Global::mqtt_client.beginMessage(MQTT::Topics::Literals::ERROR);
			Global::mqtt_client.print((const char*)message);
			Global::mqtt_client.endMessage();
		}
		else
		{
			StaticString<JSON_BUFFER_SIZE> message = (StaticString<JSON_BUFFER_SIZE>)(Global::curtain);

			Global::mqtt_client.beginMessage(MQTT::Topics::Literals::STATUS);
			Global::mqtt_client.print((const char*)message);
			Global::mqtt_client.endMessage();
		}
	}
}
