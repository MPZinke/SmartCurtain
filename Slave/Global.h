
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
#define LOOP_WAIT 1024

namespace Global
{
// ———— OTHER ————
	const uint32_t IGNORE_MOVEMENT_SIMILARITY 10  // max step difference to ignore event
	const uint32_t WIGGLE_ROOM 5  // steps within ends to consider "end zones"
	const uint32_t STEPS_FOR_CALIBRATION 5  // how picky the program should be movement


};


// ———————————————————————————————————————————————————— CURTAIN INFO ————————————————————————————————————————————————————

typedef enum
{
	CLOSED = -1,
	MIDDLE,
	OPEN
} CurtainState;


// holds current information about curtain
typedef struct
{
	uint8_t calibrate;  // if the curtain has opportunity to move full span, count steps & return value
	uint8_t correct;  // if position is unexpected, go to expected position
	uint8_t direction;  // XOR for direction (to switch which way is open)
	uint32_t current_position;  // the current length according to the RPi
	uint32_t desired_position;  // desired position according to the curtain
	uint32_t length;  // overall length of the curtain [steps]
} Curtain;


// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

// ———— CONCAT STRING TO CURTAIN_NUMBER ———— 
#define DEF(x) #x

#endif
