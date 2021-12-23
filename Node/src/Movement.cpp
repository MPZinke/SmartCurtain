
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


#include "../Headers/Movement.hpp"

#include "../Headers/C_String.hpp"
#include "../Headers/Config.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Global.hpp"


namespace Movement
{
	using Config::Hardware::CLOSE_ENDSTOP;
	using Config::Hardware::OPEN_ENDSTOP;
	using Config::Hardware::ENCODER;


	// ———— SUGAR ————
	namespace CurrentPull
	{
		const bool ON = HIGH ^ Config::Hardware::SWITCH;  // the "ON"/"ACTIVATE" state for the device
		const bool OFF = !ON;  // the "OFF"/"DEACTIVATE" state for the device

		const bool CLOSE = OFF;  // solidify convention
		const bool OPEN = !CLOSE;  // solidify convention
	}


	namespace CurtainStates
	{
		const uint8_t MIDDLE = -1;
		const uint8_t CLOSE = (uint8_t)CurrentPull::CLOSE;
		const uint8_t CLOSED = CLOSE;  // alias for sugar
		const uint8_t OPEN = (uint8_t)CurrentPull::OPEN;
	}


	using namespace CurtainStates;  // used CurtainStates as enum


	// ———————————————————————————————————————————————————— GPIO ———————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ———————————————————————————————————————————————— GPIO::WRITE  ———————————————————————————————————————————————— //

	inline void disable_motor()
	{
		digitalWrite(Config::Hardware::ENABLE_PIN, CurrentPull::ON);
	}


	inline void enable_motor()
	{
		digitalWrite(Config::Hardware::ENABLE_PIN, CurrentPull::OFF);
	}


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


	// Sets the direction pin of the motor for the stepper driver.
	// Take the ON/OFF dirction current, the curtain's direction option. 
	inline void set_direction(CurtainState direction)
	{
		// Curtain direction can overflow 0th bit to act as a switch. 
		digitalWrite(Config::Hardware::DIRECTION_PIN, ((direction + Global::curtain.direction()) & 0b1));
	}


	// ————————————————————————————————————————————————— GPIO::READ ————————————————————————————————————————————————— //

	inline bool endstop_triggered()
	{
		return is_closed() || is_open();
	}


	bool (*function_for_side(CurtainState state))()
	{
		bool(*direction_function[2])();
		direction_function[CLOSE] = is_closed;
		direction_function[OPEN] = is_open;

		return direction_function[state];
	}


	inline bool is_closed()
	{
		return digitalRead(Config::Hardware::CLOSE_PIN);
	}


	inline bool is_open()
	{
		return digitalRead(Config::Hardware::OPEN_PIN);
	}


	// —————————————————————————————————————————————————— POSITION —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// Position1 is within an allowable difference of position2.
	// Takes two step positions, and an allowable difference.
	// Returns whether they are within a certain amount of eachother.
	inline bool is_approximate_position(uint32_t position1, uint32_t position2, uint32_t allowable_difference)
	{
		return (position1 - allowable_difference <= position2) && (position2 <= position1 + allowable_difference);
	}


	// Position1 is within Gobal::wiggle_room of position2.
	// Takes two step positions.
	// Returns whether they are within a certain amount of eachother.
	inline bool is_approximate_position(uint32_t position1, uint32_t position2)
	{
		return is_approximate_position(position1, position2, Config::Curtain::POSITION_TOLLERANCE);
	}


	// —————————————————————————————————————————————— POSITION::STATE  —————————————————————————————————————————————— //

	// Approximates the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the approximated current state for position.
	inline CurtainState approximate_state_of(uint32_t position, uint32_t curtain_length)
	{
		if(is_approximate_position(position, 0)) return CLOSED;
		if(is_approximate_position(position, curtain_length)) return OPEN;
		return MIDDLE;
	}


	// Gets the state (in form CurtainState) of the curtain based on hardware.
	inline CurtainState current_hardware_state()
	{
		if(OPEN_ENDSTOP && is_open()) return OPEN;
		if(CLOSE_ENDSTOP && is_closed()) return CLOSE;
		return MIDDLE;
	}


	CurtainState current_state()
	{
		;
		if((register CurtainState hardware_state = current_hardware_state()) != MIDDLE)
		{
			return hardware_state;
		}

		else if(!Global::curtain.percentage()) return CLOSED;
		else if(Global::curtain.percentage() == 100) return OPEN;
		else return MIDDLE;
	}


	// Determines the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the current state for position.
	inline CurtainState state_of(uint32_t position, uint32_t curtain_length)
	{
		if(position == 0) return CLOSED;
		if(position == curtain_length) return OPEN;
		return MIDDLE;
	}


	// Determines the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the current state for position.
	inline CurtainState state_of(uint32_t position)
	{
		return state_of(position, Global::curtain.length());
	}


	// Determines the state of a percentage and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the current state for position.
	CurtainState state_of(uint8_t percentage)
	{
		return state_of((uint32_t)percentage, 100);
	}


	CurtainState state_of_position()
	{
		return state_of(Global::curtain.position(), Global::curtain.length());
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	uint32_t steps(Event::Event& event)
	{
		uint32_t desired_position = (uint32_t)event.percentage() * Global::curtain.length() / 100;

		// More complicated statements are processed in if clause so that if the pipeline is broken, there is less to
		//  recover to.
		if(event.direction() == OPEN)
		{
			return desired_position - Global::curtain.position();
		}
		return Global::curtain.position();
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //

	void move(Event::Event& event)
	{
		if(Global::curtain.auto_calibrate() && event.moves_full_span())
		{
			auto_calibrate(event);
		}
		// If hardware allowed and the event moves to CLOSED (including non-discrete only movement)
		else if(CLOSE_ENDSTOP && event.state() == CLOSED)
		{
			move_until_closed();
		}
		// If hardware allowed and the event moves to OPEN (including non-discrete only movement)
		else if(OPEN_ENDSTOP && event.state() == OPEN)
		{
			move_until_open();
		}
		else
		{
			uint32_t movement_steps = steps(event);

			CurtainState direction = event.direction();
			set_direction(direction);

			// Non-discrete movement is covered by Event object, and will automatically go to the else to the below if,
			//  since it is automatically converted to OPEN/CLOSED and if it had an endstop, it would be caught above.
			// If has ability to prevent self-destructive behavior
			if((OPEN_ENDSTOP && direction == OPEN) || (CLOSE_ENDSTOP && direction == CLOSE))
			{
				register uint32_t remaining_steps = step_down_to_position_or_end(movement_steps, direction);

				if(remaining_steps > Config::Curtain::POSITION_TOLLERANCE && Global::curtain.auto_correct())
				{
					if(state)
					// determine steps to move backwards

					// move steps
				}
			}
			// Shoot first, ask questions later (no hardware to prevent self-destructive behavior, so just guess)
			else
			{
				step(movement_steps);
			}
			// Move to alleged OPEN/CLOSED position

			// call move function
		}

	}


	void auto_calibrate(Event::Event& event)
	{
		bool (*function)() = function_for_side(!event.state());
		uint32_t steps = step_and_count_to_position_or_end(function);
		Global::curtain.length(steps);
	}


	void reset()
	{
		//TODO:
	}


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


	void step(const bool direction, register uint32_t steps)
	{
		set_direction(direction);

		enable_motor();
		// make number of steps an even amount to match movement loop (prevent overflow)
		step(steps);
	}



	//TODO: determine if necessary
	inline void step(register uint32_t steps)
	{
		steps &= 0xFFFFFFFE;
		while(steps)
		{
			pulse_twice();
			steps -= 2;
		}
		disable_motor();
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
	uint32_t step_and_count_to_position_or_end(bool(*state_function)())
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
	uint32_t step_down_to_position_or_end(register uint32_t steps, bool (*state_function)())
	{
		enable_motor();

		steps = steps & 0xFFFFFFFE;  // make number of steps an even amount to match movement loop (prevent overflow)
		while(steps && !state_function())
		{
			pulse_twice();
			steps -= 2;
		}

		disable_motor();
		return steps;
	}


	// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// Checks whether an endstop has been reached after taking each step.
	// Takes number of steps, a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until all steps take or sensor is tripped.
	// Returns remaining steps.
	uint32_t step_down_to_position_or_end(register uint32_t steps, CurtainState direction)
	{
		bool (*state_function)() = function_for_side(direction);
		step_down_to_position_or_end(steps, state_function);
	}


	// // DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// // Sets direction, moves to an end until sensor is tripped, counting steps along the way.
	// // Takes a direction, and optionsal state function pointer used to determine whether the sensor is tripped.
	// // Does two pulses every iteration until sensor is tripped, summing up pulses as it goes.
	// // Returns number of steps taken to reach point.
	// uint32_t set_direction_move_and_count_until_state_reached(const bool direction, bool(*state_function)())
	// {
	// 	set_direction(direction);

	// 	return move_and_count_until_state_reached(state_function);
	// }


	// ——————————————————————————————————————————————————— EVENTS ——————————————————————————————————————————————————— //

// #if CLOSE_ENDSTOP || OPEN_ENDSTOP
// 	void initial_positioning()
// 	{
// 	#if CLOSE_ENDSTOP && OPEN_ENDSTOP
// 		const bool initial_direction = CLOSE;
// 		bool opposite_direction = !initial_direction;

// 		// move completely closed or open, counting steps (for original position to move back to)
// 		uint32_t initial_steps = move_and_count_until_state_reached(initial_direction);

// 		// Set DIRECTION_SWITCHed from expected to actual. Expected direction can change based on default direction. EG,
// 		//  curtain tries to move to CLOSE, but actually moves to open, so it turns on the direction switch to make OPEN
// 		//  open and CLOSE close.
// 		if(current_state() != (int8_t)initial_direction)
// 		{
// 			Settable::DIRECTION_SWITCH = !Settable::DIRECTION_SWITCH;
// 			// opposite_direction was actually set to desired initial; switch it too
// 			opposite_direction = !opposite_direction;
// 		}

// 		// move to other side, counting steps (for curtain length)
// 		uint32_t curtain_length = move_and_count_until_state_reached(opposite_direction);

// 		// move back to original position
// 		step(curtain_length - initial_steps, !opposite_direction);

// 	// Rely on direction switch to be accurate
// 	#elif CLOSE_ENDSTOP
// 		// move completely closed, counting steps (for original position to move back to)
// 		uint32_t initial_steps = move_and_count_until_state_reached(is_closed);

// 		// move back to original position
// 		step(initial_steps);

// 	// Rely on direction switch to be accurate
// 	#elif OPEN_ENDSTOP
// 		// move completely closed, counting steps (for original position to move back to)

// 		// move back to original position
// 	#endif

// 	}
// #endif

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

	// // Move across curtain, counting steps as it goes.
	// // Takes movement direction flag option (that is then XOR-ed).
	// // Determines the curtain state for setting direction & end stop check function.
	// // Sets direction, then call movement until state reached.
	// // Returns number of steps taken.
	// uint32_t calibrate_to_opposite(bool curtain_direction)
	// {
	// 	CurtainState state = current_state();
	// 	if(!state) /* panic */ return 0 - Config::Curtain::POSITION_TOLLERANCE - 1;  // default to theoretical max

	// 	set_direction(state == OPEN ? CLOSE : OPEN);
	// 	return move_and_count_until_state_reached(state == OPEN ? is_closed : is_open);
	// }

}