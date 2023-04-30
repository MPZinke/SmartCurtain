
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


#include "../Headers/Exception.hpp"

#include "../Headers/Config.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/Message.hpp"


namespace Exception
{
	// ————————————————————————————————————————————————— EXCEPTION  ————————————————————————————————————————————————— //

	Exception::Exception(uint32_t line, String file, String message)
	{
		_line = line;
		_file = file;
		_message = message;
	}


	Exception::Exception(uint32_t line, String file, const char* message)
	{
		_line = line;
		_file = file;
		_message = String(message);
	}


	Exception::~Exception()
	{
		Global::exception = NULL;
	}


	uint32_t Exception::line()
	{
		return _line;
	}


	String Exception::file()
	{
		return _file;
	}


	Exception::operator String()
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject error_object = json_document.to<JsonObject>();

		error_object["error"] = _message;
		error_object["line"] = _line;
		error_object["file"] = _file;

		return Message::convert_JsonObject_to_String(error_object);
	}


	void Exception::send()
	{
		String message = (String)(*this);
		Global::mqtt_client.beginMessage(Message::Literal::MQTT::HUB_ERROR_TOPIC);
		Global::mqtt_client.print(message);
		Global::mqtt_client.endMessage();
		delete this;
	}
}