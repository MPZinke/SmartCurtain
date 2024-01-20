
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


#include "../Headers/Control.hpp"
#include "../Headers/Global.hpp"


#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/MQTT.hpp"


using namespace Exception;


namespace Control
{
	void loop()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		MQTT::update_hub();

		while(1)
		{
			Global::mqtt_client.poll();
		}
	}


	void process_message(int message_size)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		String topic = Global::mqtt_client.messageTopic();
		String type = topic.substring(topic.lastIndexOf('/'));

		if(JSON_BUFFER_SIZE < message_size)
		{
			Global::mqtt_client.flush();
			new Exception::Exception(__LINE__, __FILE__, "Data length is too large for JSON Buffer");
		}

		else if(type == MQTT::Topics::Literals::Parts::MOVE)
		{
			case_move(message_size);
		}
		else if(type == MQTT::Topics::Literals::Parts::UPDATE)
		{
			// Does not update hub to prevent possible ciruclar looping from possible bad programming :)
			case_update_curtain(message_size);
		}

		MQTT::update_hub();
	}


	// ——————————————————————————————————————————————————— CASES  ——————————————————————————————————————————————————— //

	inline void case_move(int message_size)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(Global::curtain.is_moving())
		{
			new Exception::Exception(__LINE__, __FILE__, "The curtain is already moving");
			return;
		}

		DeserializedJSON::DeserializedJSON event_json = MQTT::read_message(message_size);
		if(!event_json.ok())
		{
			new Exception::Exception(__LINE__, __FILE__, "Could not parse message as JSON");
			return;
		}

		if(!Event::validate(event_json))
		{
			new Exception::Exception(__LINE__, __FILE__, "Could not parse message as JSON");
			return;
		}

		static Event::Event event(event_json);
		Global::curtain.is_moving(true);
		// function, name, stack size, send the bytes as a parameter, priority, task handler, core (0, 1)
		xTaskCreatePinnedToCore((TaskFunction_t)Movement::move, "Move", 10000, (void*)&event, 1, NULL, 1);
	}


	inline void case_update_curtain(int message_size)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		DeserializedJSON::DeserializedJSON update_json = MQTT::read_message(message_size);
		if(!update_json.ok())
		{
			return;
		}

		Global::curtain = update_json;
	}
}
