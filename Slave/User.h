
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on ..                                                                                                              *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
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
	const char master_host_str[] = "10.0.0.11";
	const char page[] = "/test.php";
	const uint8_t curtain_mac[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}

	const uint16_t loop_wait = 1024;  // a nice power of 2
}


#endif
