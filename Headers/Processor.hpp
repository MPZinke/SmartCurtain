
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
	void loop();
	void process_message(int);

	void case_move(int message_size);
	void case_status();
	void case_update(int message_size);
}
