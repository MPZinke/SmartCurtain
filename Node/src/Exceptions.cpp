
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


#include "../Headers/C_String.hpp"
#include "../Headers/Exceptions.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/Request.hpp"


namespace Exceptions
{
	// ————————————————————————————————————————————————— EXCEPTION  ————————————————————————————————————————————————— //

	Exception::Exception(uint32_t line, String file, String message)
	{
		_line = line;
		_file = file;
		_message = message;
	}


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
		return String("{\"error\": \"") + _message + "\"}";
	}


	void Exception::send()
	{

	}


	// ——————————————————————————————————————————————— HTTP EXCEPTION ——————————————————————————————————————————————— //

	HTTP_Exception::HTTP_Exception(uint32_t line, String file, String message, uint16_t status_code)
	: Exception{Exception(line, file, message)}, _status_code{status_code}
	{
		Global::exception = this;
	}


	HTTP_Exception::~HTTP_Exception()
	{
		Global::exception = NULL;
	}


	String HTTP_Exception::message()
	{
		return String("{\"error\": \"") + _message + "\", \"status_code\":" + _status_code + "}";
	}


	BAD_REQUEST_400_Exception::BAD_REQUEST_400_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 400}
	{}

	NOT_FOUND_404_Exception::NOT_FOUND_404_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 404}
	{}

	INTERNAL_SERVER_ERROR_500_Exception::
	  INTERNAL_SERVER_ERROR_500_Exception(uint32_t line, String file, String message)
	: HTTP_Exception{line, file, message, 500}
	{}
}