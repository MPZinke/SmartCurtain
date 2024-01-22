
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
	: _line{line}, _file{file}, _message{String(message)}
	{
		Serial.println("Creating exception");

		if(Global::exception != NULL)
		{
			Serial.println("Calling reset_function");
			reset_function();
		}

		Global::exception = this;
		Serial.println("Created exception");
	}


	Exception::~Exception()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		Serial.println("Deleting _message");
		Serial.println("Deleted _message");
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
