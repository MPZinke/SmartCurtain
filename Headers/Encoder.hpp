
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


#include "Arduino.h"


#include "C_String.hpp"
#include "Config.hpp"
#include "Global.hpp"


#define ENCODER_READ digitalRead(Config::Hardware::ENCODER_PINA) << 1 | digitalRead(Config::Hardware::ENCODER_PINB)


namespace Encoder
{
	void encoder_loop(void*);
	bool is_positive_direction(uint8_t previous_state, uint8_t current_state);
}
