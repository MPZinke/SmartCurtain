
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.10                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "Headers/Movement.hpp"


namespace Movement
{
	// Pulse motor (HIGH->LOW) twice.
	inline void pulse_twice()
	{
		digitalWrite(Config::Hardware::PULSE_PIN, HIGH);
		delayMicroseconds(Config::Hardware::PULSE_WAIT);
		digitalWrite(Config::Hardware::PULSE_PIN, LOW);
		delayMicroseconds(Config::Hardware::PULSE_WAIT);

		digitalWrite(Config::Hardware::PULSE_PIN, HIGH);
		delayMicroseconds(Config::Hardware::PULSE_WAIT);
		digitalWrite(Config::Hardware::PULSE_PIN, LOW);
		delayMicroseconds(Config::Hardware::PULSE_WAIT);
	}


	// ————————————————————————————————————————————————— ABSTRACTIONS —————————————————————————————————————————————————

	void disable_motor()
	{
		digitalWrite(Config::Hardware::ENABLE_PIN, CurrentPull::ON);
	}


	void enable_motor()
	{
		digitalWrite(Config::Hardware::ENABLE_PIN, CurrentPull::OFF);
	}


	// Sets the direction pin of the motor for the stepper driver.
	// Take the ON/OFF dirction current, the curtain's direction option. 
	void set_direction(bool direction_current)
	{
		// Curtain direction can overflow 0th bit to act as a switch. 
		digitalWrite(Config::Hardware::DIRECTION_PIN,
		  ((direction_current + CurrentPull::direction_switch) & 0b1));
	}


	// ——————————————————————————————————————————————————— STATE  ——————————————————————————————————————————————————— //

	// Gets the state (in form CurtainState) of the curtain based on hardware.
	CurtainState current_state()
	{
		if(is_open()) return OPEN;
		if(is_closed()) return CLOSE;
		return MIDDLE;
	}


	inline bool endstop_triggered()
	{
		return is_closed() || is_open();
	}


	inline bool is_closed()
	{
		return digitalRead(Config::Hardware::CLOSE_PIN);
	}


	inline bool is_open()
	{
		return digitalRead(Config::Hardware::OPEN_PIN);
	}


	bool (*function_for_side(bool open_close_value))()
	// bool (*function_for_side)() (bool open_close_value)
	{
		bool(*direction_function[2])();
		direction_function[CLOSE] = is_closed;
		direction_function[OPEN] = is_open;

		return direction_function[open_close_value];
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	uint32_t steps_for_direction(bool direction, uint32_t current_position, uint32_t desired_position)
	{
		if(direction == CLOSE) return current_position - desired_position;
		return desired_position - current_position;
	}


	// ——————————————————————————————————————————————————— MOVEMENT ———————————————————————————————————————————————————

	// // Moves the curtain to its desired position based on curtain object data (with auto correct). The function is used
	// //  for when the curtain is not moving to an end location.
	// // Takes a reference to a curtain Object.
	// // Determines direction to move & sets it. Determines steps to move. If end state reached & auto correct option is 
	// // triggered, curtain moves to its alleged desire state (but does not auto correct from there).
	// // Return whether desired position was reached without touching end (again, not supposed to touch end state, which
	// // is what auto correct is for).
	// bool move(Curtain::Curtain& curtain)
	// {
	// 	#if CLOSE_ENDSTOP || ENCODER || OPEN_ENDSTOP


	// 	#else


	// 	#endif
	// 	if(!curtain.is_smart())
	// 	{
	// 		bool direction = curtain.event().position() ? OPEN : CLOSE;
	// 		set_direction(direction);
	// 		delayMicroseconds(Config::Hardware::PULSE_WAIT);

	// 		uint32_t steps = curtain.length();
	// 		move_motor_step_count(steps);
	// 	}
	// 	else
	// 	{
	// 		bool direction = curtain.event().position() < curtain.current_position() ? CLOSE : OPEN;
	// 		set_direction(direction);

	// 		uint32_t steps = steps_for_direction(direction, curtain.current_position(), curtain. desired_position());
	// 		if(sensor_triggered_moving_steps(steps, direction == CLOSE ? is_closed : is_open))
	// 		{
	// 			if(!curtain.correct()) return false;  // curtain ended up in a bad place and was not allowed to correct

	// 			// try again
	// 			curtain.current_position(is_open() * curtain.length());
	// 			direction = !direction;  // previous direction triggered went too far; go other direction
	// 			steps = steps_for_direction(direction, curtain.current_position(), curtain. desired_position());
	// 			return !sensor_triggered_moving_steps(steps, direction == CLOSE ? is_closed : is_open);
	// 		}
	// 	}

	// 	return true;
	// }


	void move_steps(const bool direction, register uint32_t steps)
	{
		set_direction(direction);

		enable_motor();
		// make number of steps an even amount to match movement loop (prevent overflow)
		steps &= 0xFFFFFFFE;
		while(steps)
		{
			pulse_twice();
			steps -= 2;
		}
		disable_motor();
	}



	//TODO: determine if necessary
	void move_steps(register uint32_t steps)
	{
		
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
			pulse_twice();
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
			pulse_twice();
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
			pulse_twice();
			steps -= 2;
		}

		disable_motor();
		return steps > 0;
	}


	// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// Sets direction, moves to an end until sensor is tripped, counting steps along the way.
	// Takes a direction, and optionsal state function pointer used to determine whether the sensor is tripped.
	// Does two pulses every iteration until sensor is tripped, summing up pulses as it goes.
	// Returns number of steps taken to reach point.
	uint32_t set_direction_move_and_count_until_state_reached(const bool direction, bool(*state_function)())
	{
		set_direction(direction);

		return move_and_count_until_state_reached(state_function);
	}


	// ——————————————————————————————————————————————————— EVENTS ——————————————————————————————————————————————————— //

#if CLOSE_ENDSTOP || OPEN_ENDSTOP
	void initial_positioning()
	{
	#if CLOSE_ENDSTOP && OPEN_ENDSTOP
		const bool initial_direction = CLOSE;
		bool opposite_direction = !initial_direction;

		// move completely closed or open, counting steps (for original position to move back to)
		uint32_t initial_steps = move_and_count_until_state_reached(initial_direction);

		// Set direction_switched from expected to actual. Expected direction can change based on default direction. EG,
		//  curtain tries to move to CLOSE, but actually moves to open, so it turns on the direction switch to make OPEN
		//  open and CLOSE close.
		if(current_state() != (int8_t)initial_direction)
		{
			Settable::direction_switch = !Settable::direction_switch;
			// opposite_direction was actually set to desired initial; switch it too
			opposite_direction = !opposite_direction;
		}

		// move to other side, counting steps (for curtain length)
		uint32_t curtain_length = move_and_count_until_state_reached(opposite_direction);

		// move back to original position
		move_steps(curtain_length - initial_steps, !opposite_direction);

	// Rely on direction switch to be accurate
	#elif CLOSE_ENDSTOP
		// move completely closed, counting steps (for original position to move back to)
		uint32_t initial_steps = move_and_count_until_state_reached(is_closed);

		// move back to original position
		move_steps(initial_steps);

	// Rely on direction switch to be accurate
	#elif OPEN_ENDSTOP
		// move completely closed, counting steps (for original position to move back to)

		// move back to original position
	#endif

	}
#endif

	// ————————————————————————————————————————————————————— SUGAR —————————————————————————————————————————————————————

	// For functionality, see: Movement::move_until_state_reached(.).
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
		CurtainState state = current_state();
		if(!state) /* panic */ return 0 - Config::Curtain::POSITION_TOLLERANCE - 1;  // default to theoretical max

		set_direction(state == OPEN ? CLOSE : OPEN);
		return move_and_count_until_state_reached(state == OPEN ? is_closed : is_open);
	}

}