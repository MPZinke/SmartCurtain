
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.11.30                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include "Config.h"


#define ENCODER_READ digitalRead(Config::Hardware::ENCODER_PINA) << 1 | digitalRead(Config::Hardware::ENCODER_PINB)


namespace Encoder
{
	// DESCR: Determines whether the recorded movement is +/-.
	// PARAMS: Takes the previous state, current state as 0bAB, where A & B are booleans of sensors.
	// DETAIL:
	//  Waves:   ___     ___     ___     ___     _
	//    A:   _|   |___|   |___|   |___|   |___|
	//             ___     ___     ___     ___
	//    B:   ___|   |___|   |___|   |___|   |___
	//   With using the bit format: 0bAB, where A is the A state and B is the B state, power states are:
	//    POSITIVE: 00 -> 10 -> 11 -> 01 -> 00 -> ...
	//    NEGATIVE: 00 -> 01 -> 11 -> 10 -> 00 -> ...
	//   State logic (H: High(1), L: Low(0)):
	//    ________________________________________________________________
	//   |    |           By Slit              |||         By Time        |
	//   |____|________________________________|||________________________|
	//   |----|--------------------------------|||------------------------|
	//   |    |   POSITIVE    ||   NEGATIVE    ||| POSITIVE  || NEGATIVE  |
	//   |    | A1 A2 | B1 B2 || A1 A2 | B1 B2 ||| AB1 | AB2 || AB1 | AB2 |
	//   |    |-------|-------||-------|-------|||-----|-----||-----|-----|
	//   | 1. | L  H  | L  L  || L  H  | H  H  ||| LL  | HL  || LH  | HH  |
	//   | 2. | H  H  | L  H  || H  H  | H  L  ||| HL  | HH  || HH  | HL  |
	//   | 3. | H  L  | H  H  || H  L  | L  L  ||| HH  | LH  || HL  | LL  |
	//   | 4. | L  L  | H  L  || L  L  | L  H  ||| LH  | LL  || LL  | LH  |
	//   |____|_______|_______||_______|_______|||_____|_____||_____|_____|
	//
	//  Conditions that will not happen under normal operation:
	//    ___________________________________________________________
	//   |             By Slit            |||        By Time         |
	//   |________________________________|||________________________|
	//   |     Jumps     ||   No Change   |||   Jumps   || No Change |
	//   |_______________||_______________|||___________||___________|
	//   |---------------||---------------|||-----------||-----------|
	//   | A1 A2 | B1 B2 || A1 A2 | B1 B2 ||| AB1 | AB2 || AB1 | AB2 |
	//   |-------|-------||-------|-------|||-----|-----||-----|-----|
	//   | H  L  | L  H  || H  H  | L  L  ||| HL  | LH  || HL  | HL  |
	//   | L  H  | H  L  || L  L  | H  H  ||| LH  | HL  || LH  | LH  |
	//   | H  L  | H  L  || H  H  | H  H  ||| HH  | LL  || HH  | HH  |
	//   | L  H  | L  H  || L  L  | L  L  ||| LL  | HH  || LL  | LL  |
	//   |_______|_______||_______|_______|||_____|_____||_____|_____|
	// RETURN: Whether the direction is +/- (+: true, -: false).
	bool is_positive_direction(uint8_t previous_state, uint8_t current_state)
	{
		bool A1 = (previous_state & 0b10) >> 1;  // A from the previous status
		bool A2 = (current_state & 0b10) >> 1;  // A from the current status

		bool B1 = previous_state & 0b1;  // B from the previous status
		bool B2 = current_state & 0b1;  // B from the current status

		// Return whether data matches POSITIVE 1/3 || 2/4 ;
		if((A1 != A2 && A1 == B1 && A1 == B2) || (A1 == A2 && A1 != B1 && A1 == B2))
		{
			return true;
		}
		// Return whether data matches NEGATIVE 1/3 || 2/4 ;
		else if((A1 != A2 && A1 != B1 && A1 != B2) || (A1 == A2 && A1 == B1 && A1 != B2))
		{
			return false;
		}
		else
		{
			// RAISE EXCPETION: Encoder is not working properly (see above conditions that will not happen)
		}
	}


	void encoder_loop()
	{
		uint32_t last_movement_time = 0; 
		bool waiting_to_update_after_manual_movement = false;
		uint8_t previous_state = ENCODER_READ;

		while(true)
		{
			uint8_t current_state = ENCODER_READ;

			if(previous_state != current_state)
			{
				uint32_t current_time = millis();
				if(current_time - last_movement_time >= Config::Hardware::MANUAL_MOVEMENT_INTERVAL)
				{

				}
				// Manual move
				if(!Global::is_engaged)
				{
					// determine direction
					// determine steps
					// increment/decrement Global::current_position
				}

				// set previous_state
				last_movement_time = millis();
				previous_state = current_state;
			}
			// if enough stationary time has passed
			else if()
			{
				last_movement_time = millis();
			}

			delay(Config::Hardware::ENCODER_WAIT);
		}
	}
}
