
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
	// ————————————————————————————————————————————————————— USER —————————————————————————————————————————————————————

	#define MASTER_HOST 10.0.0.11
	


	// —————————————————————————————————————————————————— ENVIROMENT ——————————————————————————————————————————————————


	// ————————————————————————————————————————————————— CURTAIN INFO —————————————————————————————————————————————————

	#define CALIBRATE 2  // bits of bool values for options in transmission. bit-and-ed with options char
	#define CORRECT 4  // bits of bool values for options in transmission. bit-and-ed with options char
	#define DIRECTION 8  // bits of bool values for options in transmission. bit-and-ed with options char

	// holds current information about curtain
	typedef struct Transmission
	{
		// options::calibrate—if the curtain has opportunity to move full span, count steps & return value
		// options::correct—if position is unexpected, go to expected position
		// options::direction—XOR for direction (to switch which way is open)
		uint8_t options;  // see above :)
		uint8_t current_position;  // the current length according to the RPi
		uint32_t desired_position;  // desired position according to the curtain
		uint32_t length;  // overall length of the curtain [steps]
	} Transmission;

	// ————————————————————————————————————————————————— TRANSMISSION —————————————————————————————————————————————————

	#define PACKET_LENGTH 10

#endif
