
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on ..                                                                                                              *
*                                                                                                                      *
*   DESCRIPTION:                                                                                                       *
*     CONVENTION:                                                                                                      *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/

#ifndef _GPIO_
#define _GPIO_

#include "Global.h"

namespace GPIO
{
	// ————————————————————————————————————————————————— GPIO: GLOBAL —————————————————————————————————————————————————

	// ———— HARDWARE ————
	const uint8_t CLOSE_PIN = 0;
	const uint8_t DIRECTION_PIN = 0;
	const uint8_t ENABLE_PIN = 0;
	const uint8_t OPEN_PIN = 0;
	const uint8_t PULSE_PIN = 0;

	const uint8_t HALL_EFFECT_IGNORED_PRECISION = 4;
	const uint8_t MIN_HALL_EFFECT_TRUE_VALUE = 255 >> HALL_EFFECT_IGNORED_PRECISION;  // min needed to be "ON"
	const uint16_t PULSE_WAIT = 16;
	const bool SWITCH = true;  // true = LOW is ON or false = HIGH is ON (depends on electronic current directions)

	// ———— SUGAR ————
	const bool ON = HIGH ^ SWITCH;  // the "ON"/"ACTIVATE" state for the device
	const bool OFF = !ON;  // the "OFF"/"DEACTIVATE" state for the device
	const bool CLOSE = OFF;  // solidify convention
	const bool OPEN = !CLOSE;  // solidify convention


	// ————————————————————————————————————————————————— ABSTRACTIONS —————————————————————————————————————————————————

	void disable_motor()
	{
		digitalWrite(ENABLE_PIN, ON);
	}


	void enable_motor()
	{
		digitalWrite(ENABLE_PIN, OFF);
	}


	void set_direction(bool direction_current, bool curtain_direction)
	{
		// "AND YOU GET AN XOR, AND YOU AN XOR, EVERYONE GETS XORS"
		digitalWrite(DIRECTION_PIN, direction_current ^ curtain_direction);
	}


	bool is_closed()
	{
		return analogRead(CLOSE_PIN) >> HALL_EFFECT_IGNORED_PRECISION > MIN_HALL_EFFECT_TRUE_VALUE;
	}


	bool is_open()
	{
		return analogRead(OPEN_PIN) >> HALL_EFFECT_IGNORED_PRECISION > MIN_HALL_EFFECT_TRUE_VALUE;
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	Curtain::CurtainState state()
	{
		if(is_open()) return Curtain::OPEN;
		if(is_closed()) return Curtain::CLOSED;
		return Curtain::MIDDLE;
	}


	// ——————————————————————————————————————————————————— MOVEMENT ———————————————————————————————————————————————————

	bool move(Curtain::Curtain& curtain, bool count_steps=false)
	{
		set_direction(curtain.desired_position() < curtain.current_position() ? CLOSE : OPEN, curtain.direction());

		if(curtain.correct())
		{
			return false;  //TODO
		}
		else
		{
			return true;
		}
	}


	// DESTRUCTIVE IF USED INCORRECTLY.
	// Moves to an end until sensor is tripped.
	// Takes a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until sensor is tripped.
	void move_until_state_reached(bool(*state_function)())
	{
		while(!state_function())
		{
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
		}
	}


	// DESTRUCTIVE IF USED INCORRECTLY.
	// Moves to an end until sensor is tripped, counting steps along the way.
	// Takes a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until sensor is tripped, summing up pulses as it goes.
	uint32_t move_and_count_until_state_reached(bool(*state_function)())
	{
		register uint32_t steps = 0;
		while(!state_function())
		{
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			steps += 2;
		}
		return steps;
	}


	// ————————————————————————————————————————————————————— SUGAR —————————————————————————————————————————————————————

	void move_until_closed(bool curtain_direction)
	{
		set_direction(CLOSE, curtain_direction);
		move_until_state_reached(is_closed);
	}


	void move_until_open(bool curtain_direction)
	{
		set_direction(OPEN, curtain_direction);
		move_until_state_reached(is_open);
	}


	// ——————————————————————————————————————————————————— TRACKING ———————————————————————————————————————————————————

	uint32_t calibrate_to_opposite(bool curtain_direction)
	{
		Curtain::CurtainState current_state = state();
		if(!current_state) /* panic */ return 0 - Global::wiggle_room - 1;  // default to theoretical max

		set_direction(current_state == Curtain::OPEN ? CLOSE : OPEN, curtain_direction);
		return move_and_count_until_state_reached(current_state == Curtain::OPEN ? is_closed : is_open);
	}

}  // end namespace GPIO

#endif
