
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


#ifndef _USER_
#define _USER_

namespace User
{
	// ————————————————————————————————————————————————— USER: GLOBAL —————————————————————————————————————————————————

	const char curtain_number[] = "1";
	const char master_host[] = {10,0,0,11};
	const char master_host_cstr[] = "10.0.0.11";
	const char current_page[] = "/api/current";  // page to get status of curtain from.
	const char complete_page[] = "/api/complete";  // page to update Hub to current status
	const uint8_t curtain_mac[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}

	const uint16_t loop_wait = 1024;  // a nice power of 2

} // end namespace User

#endif
