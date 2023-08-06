
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


#include <WiFiClient.h>


#include "../Headers/Global.hpp"


#include "../Headers/Curtain.hpp"


namespace Global
{
	Curtain::Curtain curtain;

	WiFiClient wifi_client;
	MqttClient mqtt_client(wifi_client);

	Exception::Exception* exception = NULL;
} // end namespace Global
