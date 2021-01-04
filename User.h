
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


namespace User
{
	// ————————————————————————————————————————————————— USER: GLOBAL —————————————————————————————————————————————————

	// ———— ETHERNET ————
	// Node
	uint8_t mac_address[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}
	uint8_t node_host[] = {10,0,0,12};
	const uint16_t port = 80;
	// Router
	uint8_t router_gateway[] = {10,0,0,1};
	uint8_t subnet_mask[] = {255,255,255,0};  // of the router

	uint8_t hub_host[] = {10, 0, 0, 23};
	const char hub_host_cstr[] = "10.0.0.23";

} // end namespace User

#define USER_COMPLETE_PAGE "/test"
