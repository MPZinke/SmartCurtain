
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


#include "../Headers/Curtain.hpp"
#include "../Headers/DeserializedJSON.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/Message.hpp"


using namespace Exception;


namespace Processor
{
	void loop()
	{
		while(1)
		{
			Global::mqtt_client.poll();
		}
	}


	void process_message(int message_size)
	{
		String topic = Global::mqtt_client.messageTopic();
		String type = topic.substring(topic.lastIndexOf('/'));

		if(JSON_BUFFER_SIZE < message_size)
		{
			Global::mqtt_client.flush();
			new Exception::Exception(__LINE__, __FILE__, "Data length is too large for JSON Buffer");
		}

		else if(type == Message::Literal::MQTT::MOVE_SUFFIX)
		{
			case_move(message_size);
		}
		else if(type == Message::Literal::MQTT::STATUS_SUFFIX)
		{
			case_status();
		}
		else if(type == Message::Literal::MQTT::UPDATE_SUFFIX)
		{
			case_update(message_size);
		}

		if(Global::exception)
		{
			Global::exception->send();
		}
	}


	// ——————————————————————————————————————————————————— CASES  ——————————————————————————————————————————————————— //

	void case_move(int message_size)
	{
		if(Global::curtain.is_moving())
		{
			new Exception::Exception(__LINE__, __FILE__, "The curtain is already moving");
			return;
		}

		DeserializedJSON::DeserializedJSON event_json = Message::read_message(message_size);
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

		Event::Event event(event_json);
		Global::curtain.is_moving(true);
		// function, name, stack size, send the bytes as a parameter, priority, task handler, core (0, 1)
		xTaskCreatePinnedToCore((TaskFunction_t)Movement::move, "Move", 10000, (void*)&event, 1, NULL, 1);
	}


	void case_status()
	{
		Message::update_hub();
	}


	void case_update(int message_size)
	{
		DeserializedJSON::DeserializedJSON update_json = Message::read_message(message_size);
		if(!update_json.ok())
		{
			return;
		}

		Global::curtain = update_json;
		Message::update_hub();
	}
}
