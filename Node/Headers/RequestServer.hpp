
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


namespace RequestServer
{
	void server_loop();

	StaticJsonDocument<JSON_BUFFER_SIZE> decode_json();
	void case_move(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
	void case_update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
}
