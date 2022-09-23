
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


#include "../Headers/Exceptions.hpp"

#include "../Headers/Global.hpp"

#include "../Headers/Message.hpp"


namespace Exceptions
{
	// ————————————————————————————————————————————————— EXCEPTION  ————————————————————————————————————————————————— //

	Exception::Exception(uint32_t line, String file, String message)
	{
		_line = line;
		_file = file;
		_message = message;
	}


	Exception::~Exception()
	{}


	uint32_t Exception::line()
	{
		return _line;
	}


	String Exception::file()
	{
		return _file;
	}


	String Exception::message()
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
		String message = this->message();
		Message::respond_with_json_and_stop(message);
		Serial.println(message);  //TESTING
		delete this;
	}


	// ——————————————————————————————————————————————— HTTP EXCEPTION ——————————————————————————————————————————————— //

	HTTP_Exception::HTTP_Exception(uint32_t line, String file, String message, uint16_t status_code,
	  const char* request_header)
	: Exception{Exception(line, file, message)}, _status_code{status_code}, _request_header{request_header}
	{
		Global::exception = this;
	}


	HTTP_Exception::~HTTP_Exception()
	{
		Global::exception = NULL;
	}


	String HTTP_Exception::message()
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject error_object = json_document.to<JsonObject>();
		
		error_object["error"] = _message;
		error_object["status code"] = _status_code;
		error_object["line"] = _line;
		error_object["file"] = _file;
		
		return Message::convert_JsonObject_to_String(error_object);
	}
		
		
	void HTTP_Exception::send()
	{
		String message = this->message();
		Serial.print("_request_header: ");
		Serial.println(this->_request_header);
		Message::respond_with_json_and_stop(message, this->_request_header);
		Serial.println(message);
		delete this;
	}


	BAD_REQUEST_400_Exception::BAD_REQUEST_400_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 400, Message::Literal::HTTP::BAD_REQUEST}
	{}


	UNAUTHORIZED_401_Exception::UNAUTHORIZED_401_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 401, Message::Literal::HTTP::UNAUTHORIZED}
	{}


	FORBIDDEN_403_Exception::FORBIDDEN_403_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 403, Message::Literal::HTTP::UNAUTHORIZED}
	{}


	NOT_FOUND_404_Exception::NOT_FOUND_404_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 404, Message::Literal::HTTP::NOT_FOUND_REQUEST}
	{}


	INTERNAL_SERVER_ERROR_500_Exception::
	  INTERNAL_SERVER_ERROR_500_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 500, Message::Literal::HTTP::INTERNAL_SERVER_ERROR_REQUEST}
	{}
}