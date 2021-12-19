
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


namespace Global
{
	WiFiServer server(Config::Network::PORT);

	WiFiClient client;

	// Mutable curtain information
	uint32_t current_position = 0;  // holds current position
	uint32_t curtain_length = Config::Hardware::DEFAULT_LENGTH;

	jmp_buf jump_buffer;
} // end namespace Global
