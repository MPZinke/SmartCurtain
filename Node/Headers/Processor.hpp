
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


#include <ArduinoJson.h>


#include "Config.hpp"


namespace Processor
{
	void process_message();

	StaticJsonDocument<JSON_BUFFER_SIZE> decode_json();
	void case_default(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
	void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
	void case_status();
	void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
}
