
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


#ifndef _GLOBAL_
#define _GLOBAL_

#include "User.h"

// —————————————————————————————————————————————————————— CURTAIN ——————————————————————————————————————————————————————

namespace Curtain  // also exists in Curtain.h
{
	// ———————————————————————————————————————————————— CURTAIN: TYPES ————————————————————————————————————————————————

	typedef enum
	{
		CLOSED = -1,
		MIDDLE,
		OPEN
	} CurtainState;


	class Curtain;  // declare for GPIO.h functions, since GPIO.h is called in Curtain.h (and thus exists before it)
}


// —————————————————————————————————————————————————————— GLOBAL ——————————————————————————————————————————————————————

namespace Global
{
	const uint32_t ignore_movement_similarity = 10;  // max step difference to ignore event
	const uint32_t wiggle_room = 5;  // steps within ends to consider "end zones"
	const uint32_t steps_for_calibration = 5;  // how picky the program should be movement


	IPAddress server(User::master_host[0], User::master_host[1], User::master_host[2], User::master_host[3]);
	EthernetClient client;  // the magician
}


// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

#define DEF(x) #x  // convert a define into a string

#endif
