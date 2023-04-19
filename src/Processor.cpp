
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
			new Exception(__FILE__, __LINE__, "TODO")
			return;
		}

		String event_json = read_message(message_size);
		Global::event = Event::Event(event_json);
	}


	void case_status()
	{
		String status_string = Message::status_json();
		Global::mqtt_client.beginMessage(Message::Literal::MQTT::HUB_UPDATE_TOPIC);
		Global::mqtt_client.print(status_string);
		Global::mqtt_client.endMessage();
	}


	void case_update(int message_size)
	{
		if(!read_message(message_size))
		{
			return;
		}

		String curtain_json = read_message(message_size);
		Global::curtain.update(curtain_json);
	}
}
