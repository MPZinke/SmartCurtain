
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.28                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Handware control part of the code. The trickiest part of this page is the XOR—direction and ON/OFF    *
*       are depenent on physical setup of the motor, and the High/Low of the stepper driver.                           *
*   CONVENTION: - OPEN = ON, CLOSE = OFF                                                                               *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include "Global.h"

namespace Gpio
{
	void disable_motor();
	void enable_motor();
	void set_direction(bool);
	bool move(Curtain::Curtain&);
	bool is_closed();
	bool is_open();
	Curtain::CurtainState state();
	uint32_t steps_for_direction(bool, uint32_t, uint32_t);
	void move_until_state_reached(bool(*)());
	uint32_t move_and_count_until_state_reached(bool(*)());
	bool sensor_triggered_moving_steps(register uint32_t, bool(*)());
	void move_until_closed();
	void move_until_open();
	uint32_t calibrate_to_opposite(bool);
	void move_motor_step_count(uint32_t);


	// ————————————————————————————————————————————————— GPIO: GLOBAL —————————————————————————————————————————————————

	// ———— SUGAR ————
	const bool ON = HIGH ^ Configure::Hardware::SWITCH;  // the "ON"/"ACTIVATE" state for the device
	const bool OFF = !ON;  // the "OFF"/"DEACTIVATE" state for the device
	const bool CLOSE = OFF;  // solidify convention
	const bool OPEN = !CLOSE;  // solidify convention


	// ————————————————————————————————————————————————— ABSTRACTIONS —————————————————————————————————————————————————

	void disable_motor()
	{
		digitalWrite(Configure::Hardware::ENABLE_PIN, ON);
	}


	void enable_motor()
	{
		digitalWrite(Configure::Hardware::ENABLE_PIN, OFF);
	}


	// Sets the direction pin of the motor for the stepper driver.
	// Take the ON/OFF dirction current, the curtain's direction option. 
	void set_direction(bool direction_current)
	{
		// Curtain direction can overflow 0th bit to act as a switch. 
		digitalWrite(Configure::Hardware::DIRECTION_PIN, ((direction_current + DIRECTION_SWITCH) & 0b1));
	}


	bool is_closed()
	{
		return digitalRead(Configure::Hardware::CLOSE_PIN);
	}


	bool is_open()
	{
		return digitalRead(Configure::Hardware::OPEN_PIN);
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	// Gets the state (in form CurtainState) of the curtain based on hardware.
	Curtain::CurtainState state()
	{
		if(is_open()) return Curtain::OPEN;
		if(is_closed()) return Curtain::CLOSED;
		return Curtain::MIDDLE;
	}


	uint32_t steps_for_direction(bool direction, uint32_t current_position, uint32_t desired_position)
	{
		if(direction == CLOSE) return current_position - desired_position;
		return desired_position - current_position;
	}


	// ——————————————————————————————————————————————————— MOVEMENT ———————————————————————————————————————————————————

	// Moves the curtain to its desired position based on curtain object data (with auto correct). The function is used
	//  for when the curtain is not moving to an end location.
	// Takes a reference to a curtain Object.
	// Determines direction to move & sets it. Determines steps to move. If end state reached & auto correct option is 
	// triggered, curtain moves to its alleged desire state (but does not auto correct from there).
	// Return whether desired position was reached without touching end (again, not supposed to touch end state, which
	// is what auto correct is for).
	bool move(Curtain::Curtain& curtain)
	{
		if(!curtain.is_smart())
		{
			bool direction = curtain.desired_position() ? OPEN : CLOSE;
			set_direction(direction);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);

			uint32_t steps = curtain.length();
			move_motor_step_count(steps);
		}
		else
		{
			bool direction = curtain.desired_position() < curtain.current_position() ? CLOSE : OPEN;
			set_direction(direction);

			uint32_t steps = steps_for_direction(direction, curtain.current_position(), curtain. desired_position());
			if(sensor_triggered_moving_steps(steps, direction == CLOSE ? is_closed : is_open))
			{
				if(!curtain.correct()) return false;  // curtain ended up in a bad place and was not allowed to correct

				// try again
				curtain.current_position(is_open() * curtain.length());
				direction = !direction;  // previous direction triggered went too far; go other direction
				steps = steps_for_direction(direction, curtain.current_position(), curtain. desired_position());
				return !sensor_triggered_moving_steps(steps, direction == CLOSE ? is_closed : is_open);
			}
		}

		return true;
	}


	// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// Moves to an end until sensor is tripped.
	// Takes a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until sensor is tripped.
	void move_until_state_reached(bool(*state_function)())
	{
		enable_motor();

		while(!state_function())
		{
			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);

			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
		}

		disable_motor();
	}


	// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// Moves to an end until sensor is tripped, counting steps along the way.
	// Takes a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until sensor is tripped, summing up pulses as it goes.
	// Returns number of steps taken to reach point.
	uint32_t move_and_count_until_state_reached(bool(*state_function)())
	{
		enable_motor();

		register uint32_t steps = 0;
		while(!state_function())
		{
			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);

			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			steps += 2;
		}

		disable_motor();
		return steps;
	}


	// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// Checks whether an endstop has been reached after taking each step.
	// Takes number of steps, a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until all steps take or sensor is tripped.
	// Returns remaining steps.
	bool sensor_triggered_moving_steps(register uint32_t steps, bool(*state_function)())
	{
		enable_motor();

		steps = steps & 0xFFFFFFFE;  // make number of steps an even amount to match movement loop (prevent overflow)
		while(steps && !state_function())
		{
			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);

			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			steps -= 2;
		}

		disable_motor();
		return steps;
	}


	// ————————————————————————————————————————————————————— SUGAR —————————————————————————————————————————————————————

	// For functionality, see: Gpio::move_until_state_reached(.).
	// Takes movement direction flag option (that is then XOR-ed).
	void move_until_closed()
	{
		set_direction(CLOSE);
		move_until_state_reached(is_closed);
	}


	// For functionality, see: move_until_state_reached(.).
	// Takes movement direction flag option (that is then XOR-ed).
	void move_until_open()
	{
		set_direction(OPEN);
		move_until_state_reached(is_open);
	}


	// ——————————————————————————————————————————————————— TRACKING ———————————————————————————————————————————————————

	// Move across curtain, counting steps as it goes.
	// Takes movement direction flag option (that is then XOR-ed).
	// Determines the curtain state for setting direction & end stop check function.
	// Sets direction, then call movement until state reached.
	// Returns number of steps taken.
	uint32_t calibrate_to_opposite(bool curtain_direction)
	{
		Curtain::CurtainState current_state = state();
		if(!current_state) /* panic */ return 0 - Global::wiggle_room - 1;  // default to theoretical max

		set_direction(current_state == Curtain::OPEN ? CLOSE : OPEN);
		return move_and_count_until_state_reached(current_state == Curtain::OPEN ? is_closed : is_open);
	}


	void move_motor_step_count(register uint32_t steps)
	{
		enable_motor();
		// make number of steps an even amount to match movement loop (prevent overflow)
		steps &= 0xFFFFFFFE;
		while(steps)
		{
			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);

			digitalWrite(Configure::Hardware::PULSE_PIN, HIGH);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			digitalWrite(Configure::Hardware::PULSE_PIN, LOW);
			delayMicroseconds(Configure::Hardware::PULSE_WAIT);
			steps -= 2;
		}
		disable_motor();
	}

}  // end namespace GPIO
