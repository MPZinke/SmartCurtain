
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


namespace Global
{
	Curtain::Curtain curtain((uint8_t)C_String::atoi(Config::Curtain::CURTAIN_ID));

	WiFiClient client;
	WiFiServer server(Config::Network::PORT);

	jmp_buf jump_buffer;
} // end namespace Global
