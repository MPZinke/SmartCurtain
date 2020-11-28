
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

// ———————————————————————————————————————————————————————— USER ————————————————————————————————————————————————————————

#define CURTAIN_NUMBER 1  //EDIT
#define MASTER_HOST_ARRAY 10,0,0,11
#define MASTER_HOST_STR "10.0.0.11"
#define PAGE /test.php

#define CURTAIN_MAC {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31}  // {'Z', 'C', 'R', 'T', 'N', '1'}

// ————————————————————————————————————————————————————— ENVIROMENT —————————————————————————————————————————————————————

// ———— WAIT ————
#define HTTP_FAIL_RESPONSE_WAIT 5000
#define LOOP_WAIT 1024
#define PULSE_WAIT 16


// ———— OTHER ————
#define IGNORE_MOVEMENT_SIMILARITY 10  // max step difference to ignore event
#define WIGGLE_ROOM 5  // steps within ends to consider "end zones"
#define STEPS_FOR_CALIBRATION 5  // how picky the program should be movement


// ———————————————————————————————————————————————————— CURTAIN INFO ————————————————————————————————————————————————————

#define CALIBRATE 2  // bit of bool values for options in transmission. bit-and-ed with options char
#define CORRECT 4  // bit of bool values for options in transmission. bit-and-ed with options char
#define DIRECTION 8  // bit of bool values for options in transmission. bit-and-ed with options char

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

// ——————————————————————————————————————————————————— TRANSMISSION ———————————————————————————————————————————————————

#define PACKET_LENGTH 10  // if this ever changes, change Transmissions::message_length(.) to match number of digits
#define VALID_RESPONSE_STR "HTTP/1.1 200 OK"  // initial string for valid response from device

// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

// ———— CONCAT STRING TO CURTAIN_NUMBER ———— 
#define DEF(x) #x

#endif
