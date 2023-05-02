
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
	void(*reset_function) (void) = 0;  // Move the program counter to 0x00


	// ————————————————————————————————————————————————— EXCEPTION  ————————————————————————————————————————————————— //

	Exception::Exception(uint32_t line, String file, String message)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(Global::exception != NULL)
		{
			reset_function();
		}

		_line = line;
		_file = file;
		_message = message;

		Global::exception = this;
	}


	Exception::Exception(uint32_t line, String file, const char* message)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(Global::exception != NULL)
		{
			reset_function();
		}

		_line = line;
		_file = file;
		_message = String(message);

		Global::exception = this;
	}


	Exception::~Exception()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		Global::exception = NULL;
	}


	uint32_t Exception::line()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _line;
	}


	String Exception::file()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _file;
	}


	Exception::operator String()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject error_object = json_document.to<JsonObject>();

		error_object["error"] = _message;
		error_object["line"] = _line;
		error_object["file"] = _file;

		return Message::convert_JsonObject_to_String(error_object);
	}
}