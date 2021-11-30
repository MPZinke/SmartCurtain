
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


#define ENCODER_READ digitalRead(Config::Hardware::ENCODER_PIN1) << 2 \
  | digitalRead(Config::Hardware::ENCODER_PIN2) << 1 \
  | digitalRead(Config::Hardware::ENCODER_PIN3)


namespace Encoder
{
	void movement_tracker()
	{
		uint8_t previous_state = ENCODER_READ;
		while(true)
		{
			uint8_t current_state = ENCODER_READ;
			if(previous_state != current_state)
			{
				// Manual move
				if(!Global::is_engaged)
				{
					// determine direction
					// determine steps
					// increment/decrement Global::current_position
				}

				// set previous_state
				previous_state = current_state;
			}

			delay(Config::Hardware::ENCODER_WAIT);
		}
	}
}
