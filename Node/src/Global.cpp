
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.19                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Global.hpp"

#include "../Headers/C_String.hpp"
#include "../Headers/Curtain.hpp"
#include "../Headers/Event.hpp"


namespace Global
{
	Curtain::Curtain curtain((uint8_t)C_String::atoi(Config::Curtain::CURTAIN_ID));
	Event::Event event(0, Config::Hardware::OPEN_ENDSTOP * 100,
	  !(Config::Hardware::OPEN_ENDSTOP || Config::Hardware::CLOSE_ENDSTOP));

	WiFiServer server(Config::Network::PORT);
	WiFiClient client;

	Exceptions::Exception* exception = NULL;
	StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
} // end namespace Global
