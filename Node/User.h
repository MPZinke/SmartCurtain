
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

	const char curtain_number[] = "1";
	// ———— ETHERNET ————
	// Node
	uint8_t mac_address[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}
	uint8_t node_host[] = {10,0,0,12};
	const uint16_t port = 80;
	// Router
	uint8_t router_gateway[] = {10,0,0,1};
	uint8_t subnet_mask[] = {255,255,255,0};  // of the router

	const char hub_host_cstr[] = "10.0.0.11";
	const char current_page[] = "/api/current";  // page to get status of curtain from.
	const char complete_page[] = "/api/complete";  // page to update Hub to current status

} // end namespace User
