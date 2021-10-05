
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.27                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Page to store and edit user specific data. Moved here from Global to better indicate what the user    *
*    needs to change.                                                                                                  *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#define __ETHERNET__ false  //POPULATE: set whether Ethernet version
#if __ETHERNET__
	#define __WIFI__ false
#else
	#define __WIFI__ true
#endif


// Create hardware client based off of defined usage
#if __ETHERNET__
	#define HARDWARE_CLIENT EthernetClient
#elif __WIFI__
	#define HARDWARE_CLIENT WiFiClient
#endif


namespace User
{
	// ————————————————————————————————————————————————— USER: GLOBAL —————————————————————————————————————————————————
	const char curtain_id[] = "1";

	const uint16_t port = 80;  //POPULATE: port
	uint8_t hub_host[] = {10, 0, 0, 11};  //POPULATE: hub host IP
	const char hub_host_cstr[] = "10.0.0.11";  //POPULATE: hub host cstr
	uint8_t mac_address[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}
	// Router
	uint8_t node_host[] = {10,0,0,12};  //POPULATE: node host
	uint8_t router_gateway[] = {10,0,0,1};  //POPULATE: router gateway
	uint8_t subnet_mask[] = {255,255,255,0};  // of the router

	// ———— ETHERNET ————
#if __WIFI__
	char SSID[] = "";  //POPULATE: wifi name
	char password[] = "";  //POPULATE: wifi password
#endif

} // end namespace User

#define USER_COMPLETE_PAGE "/api/update/deactivateevent"
