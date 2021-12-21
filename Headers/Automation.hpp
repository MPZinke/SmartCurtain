
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.04                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#ifndef __AUTOMATION__
#define __AUTOMATION__


#include <ArduinoJson.h>


#include "Config.hpp"


namespace Automation
{
	void automation_loop(void*);
	void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
	StaticJsonDocument<JSON_BUFFER_SIZE> decode_json();
}


#endif
