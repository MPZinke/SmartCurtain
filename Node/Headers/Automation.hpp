
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


#include <ArduinoJson.h>


#include "Config.hpp"


namespace Automation
{
	void automation_loop(void*);
	StaticJsonDocument<JSON_BUFFER_SIZE> decode_json();
	// ———— CASES ———— //
	void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
	void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
}
