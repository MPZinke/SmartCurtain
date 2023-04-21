
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
		String type = topic.substr(topic.lastIndexOf('/'));

		if(JSON_BUFFER_SIZE < message_size)
		{
			Global::mqtt_client.flush();
			new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Data length is too large for JSON Buffer");
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
		if(!Global::event.is_activated() || Global::event.is_moving())
		{
			new Exception(__FILE__, __LINE__, "An event is already active")
			return;
		}

		Optional<StaticJsonDocument<JSON_BUFFER_SIZE>> json_document = read_message(message_size);
		if(!json_document.ok())
		{
			return
		}

		Global::event = Event::Event(event_json);
	}


	void case_status()
	{
		Message::update_hub();
	}


	void case_update(int message_size)
	{
		Optional<StaticJsonDocument<JSON_BUFFER_SIZE>> json_document = read_message(message_size);
		if(!json_document.ok())
		{
			return
		}

		Global::curtain.update(json_document.value());
		Message::update_hub();
	}
}
