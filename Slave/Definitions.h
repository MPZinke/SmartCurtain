
/***********************************************************************************************************
*
*	created by: MPZinke
*	on 2020.09.19
*
*	DESCRIPTION: Ethernet connection version of smart curtain.  Queries server and waits
*		on response for next event.  On event, drives curtain to location.  
*		**Holds preprocessor, user defined info**
*	BUGS:
*	FUTURE:
*
***********************************************************************************************************/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

	// ———— USER ————

	#define CURTAIN_NUMBER 1  //EDIT

	#define HOST 10.0.0.11
	#define PAGE /test.php


	// ———— TRANSMISSION & DECODING ————
	// designed for {{1111111111111}}, {{<1> <3> <3> <3> <3>}}
	// {{<1 modes>  <3 c_len> <3 c_pos> <3 e_id> <3 e_pos>}}
	#define BUFFER_SIZE 16  // number of characters to copy into buffer for response
	#define VALID_RESPONSE_STR "HTTP/1.1 200 OK"  // initial string for valid response from device

	#define SINGLE_SHIFT 7  // number of bits to shift byte over by (BASE = 2^X) EG (128 = 2^7)
	#define DOUBLE_SHIFT 14  // SINGLE_SHIFT doubled

	#define MODES_INDEX 1  // index in response of modes setup
	#define CALIBRATE_MASK 1  // bit value of auto calibrations (2^0)
	#define CORRECT_MASK 2  // bit value of auto correct (2^1)
	#define DIRECTION_MASK 4  // bit value of direction (2^2)
	#define C_LENGTH_INDEX 2  
	#define C_POSITION_INDEX 5
	#define E_ID_INDEX 8
	#define E_POSITION_INDEX 11
	#define END_ENCODING 14


	// ———— GPIO ————
	#define ENABLE_PIN 0  // char version of int 26
	#define DIRECTION_PIN 1  // char version of int 33
	#define PULSE_PIN 2  // char version of int 27

	#define CLOSE_PIN 14  // char version of int 13
	#define OPEN_PIN 15  // char version of int 14

	// ———— WAIT ————
	#define HTTP_FAIL_RESPONSE_WAIT 5000
	#define LOOP_WAIT 1024
	#define PULSE_WAIT 16


	// ———— OTHER ————
	#define IGNORE_MOVEMENT_SIMILARITY 10  // max step difference to ignore event
	#define WIGGLE_ROOM 5  // steps within ends to consider "end zones"
	#define STEPS_FOR_CALIBRATION 5  // how picky the program should be movement


	// ———— CONCAT STRING TO CURTAIN_NUMBER ———— 
	#define DEF(x) #x
	#define DEF_SPC_STR(x, y) DEF(x) " " #y
	#define STR_DEF(x, y) #x DEF(y)
	#define STR_SPC_DEF(x, y) #x " " DEF(y)
	#define STR_SPC_DEF_SPC_STR(x, y, z) #x " " DEF(y) " " #z
	#define STR_DEF_STR(x, y, z) #x DEF(y) #z
	#define STR_STR_DEF(x, y, z) #x #y DEF(z)



	// holds current information about curtain
	typedef struct CURTAIN
	{
		bool calibrate;  // if the curtain has opportunity to move full span, count steps & return value
		bool correct;  // if position is unexpected, go to expected position
		bool direction;  // XOR for direction (to switch which way is open)
		uint32_t length;  // overall length of the curtain [steps]
		uint32_t position;  // current position according to the curtain
	} CURTAIN;


	// hold information about current event
	typedef struct EVENT
	{
		uint32_t id;  // event ID for current event (0 means no event)
		uint32_t position;  // desired event
	} EVENT;

#endif