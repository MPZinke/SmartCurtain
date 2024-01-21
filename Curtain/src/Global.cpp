
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


#include "../Headers/Curtain.hpp"


namespace Global
{
	unsigned long last_hub_update = millis();

	Curtain::Curtain curtain;

	WiFiClient wifi_client;
	MqttClient mqtt_client(wifi_client);

	Exception::Exception* exception = NULL;
}  // end namespace Global
