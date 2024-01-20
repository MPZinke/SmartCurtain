
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
#include "../Headers/CString.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/MQTT.hpp"
#include "../Headers/StaticString.hpp"


namespace Exception
{
	void(*reset_function) (void) = 0;  // Move the program counter to 0x00


	// ————————————————————————————————————————————————— EXCEPTION  ————————————————————————————————————————————————— //

	Exception::Exception(uint32_t line, const char* file, const char* message)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	: _line{line}, _file{file}, _message{new char[CString::length(message, 0xFFFF-1)+1]}
	{
		if(Global::exception != NULL)
		{
			reset_function();
		}

		// If I write an error message that needs all 0xFFFF characters, then this deserves to fail.
		//  +1 for null terminator. -1 to allow room for null terminator room while not overflowing copy.
		CString::copy(message, (char*)_message, 0xFFFF);

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
		delete[] _message;
		Global::exception = NULL;
	}


	Exception::operator StaticString<JSON_BUFFER_SIZE>()
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

		return StaticString<JSON_BUFFER_SIZE>(error_object);
	}
}
