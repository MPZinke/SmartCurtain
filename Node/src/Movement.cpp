
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
	using Config::Hardware::BOTH_ENDSTOPS;
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
		const uint8_t UNKNOWN = -2;  // technically 0xFE
		const uint8_t MIDDLE = -1;  // technically 0xFF
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
	inline bool is_approximate_position(register uint32_t position1, register uint32_t position2,
	  register uint32_t allowable_difference)
	{
		return (position1 - allowable_difference <= position2) && (position2 <= position1 + allowable_difference);
	}


	// Position1 is within Gobal::wiggle_room of position2.
	// Takes two step positions.
	// Returns whether they are within a certain amount of eachother.
	inline bool is_approximate_position(register uint32_t position1, register uint32_t position2)
	{
		return is_approximate_position(position1, position2, Config::Curtain::POSITION_TOLLERANCE);
	}


	// —————————————————————————————————————————————— POSITION::STATE  —————————————————————————————————————————————— //

	// Approximates the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the approximated current state for position.
	inline CurtainState approximate_state_of(register uint32_t position, register uint32_t curtain_length)
	{
		if(is_approximate_position(position, 0)) return CLOSED;
		if(is_approximate_position(position, curtain_length)) return OPEN;
		return MIDDLE;
	}


	// Gets the state (in form CurtainState) of the curtain based on hardware.
	inline CurtainState current_hardware_state()
	{
		if(OPEN_ENDSTOP && is_open()) return OPEN;
		else if(CLOSE_ENDSTOP && is_closed()) return CLOSE;
		else if(BOTH_ENDSTOPS) return MIDDLE;
		else return UNKNOWN;
	}


	// SUMMARY: Gets the precises state of the hardware or the approximate state of the curtain.
	// DETAILS: Checks available hardware for their state. If the hardware is unable to precisely determine state, it
	//  gets the state based on Global::Curtain details.
	// RETURNS: The CurtainState value of the state.
	CurtainState current_state()
	{
		// Attempt to get state through hardware.
		register CurtainState curtain_state;
		if((curtain_state = current_hardware_state()) != UNKNOWN)
		{
			return curtain_state;
		}

		// Hardware does not clearly show current state, so rely on curtain.
		else if(!(curtain_state = Global::curtain.percentage())) return CLOSED;
		else if(curtain_state == 100) return OPEN;
		else return MIDDLE;
	}


	// Determines the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the current state for position.
	inline CurtainState state_of(register uint32_t position, register uint32_t curtain_length)
	{
		if(position == 0) return CLOSED;
		if(position == curtain_length) return OPEN;
		return MIDDLE;
	}


	// Determines the position and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the current state for position.
	inline CurtainState state_of(register uint32_t position)
	{
		return state_of(position, Global::curtain.length());
	}


	// Determines the state of a percentage and then returns an enum value.
	// Takes a position to check, the length of the curtain to compare it to.
	// Returns the enum value of the current state for position.
	CurtainState state_of(register uint8_t percentage)
	{
		return state_of((uint32_t)percentage, 100);
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	bool return_false()
	{
		return false;
	}


	bool return_true()
	{
		return true;
	}


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
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	void activate(register Event::Event& event)
	{
		if(Global::curtain.auto_calibrate() && event.moves_full_span())
		{
			move_and_calibrate(event);
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
		// Non-discrete, movement is covered by Event object. If the movement is discrete, it will automatically go to
		//  the else of the below if, since it is automatically converted to OPEN/CLOSED. If it had an endstop and were
		//  supposed to move to an end, the event would be caught above.
		// if statement: has ability to prevent self-destructive behavior
		else if((OPEN_ENDSTOP && event.direction() == OPEN) || (CLOSE_ENDSTOP && event.direction() == CLOSE))
		{
			move_discretely(event);
		}
		// —— No endstops below this point ——
		else
		{
			step(event);
		}
	}


	// ——————————————————————————————————————————————— MOVEMENT::MOVE ——————————————————————————————————————————————— //
	// Move implies that it will move until a possible event happens & implies it is safeguarded by hardware.
	// If you use these functions without checking the hardware exists,
	//  1) You are using it wrong
	//  2) You are going grind your belt (amongst other things)

	void move_and_calibrate(Event::Event& event)
	{
		bool (*function)() = function_for_side(!event.state());
		uint32_t steps = move_and_count_to_position_or_end(function);
		Global::curtain.length(steps);
	}


	// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// Moves to an end until sensor is tripped, counting steps along the way.
	// Takes a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until sensor is tripped, summing up pulses as it goes.
	// Returns number of steps taken to reach point.
	uint32_t move_and_count_to_position_or_end(bool(*state_function)())
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
	uint32_t move_and_count_down_or_until_end(register uint32_t steps, bool (*state_function)())
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


	// Checks whether an endstop has been reached after taking each step.
	// Takes number of steps, a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until all steps take or sensor is tripped.
	// Returns remaining steps.
	uint32_t move_and_count_down_or_until_end(register uint32_t steps, CurtainState direction)
	{
		bool (*state_function)() = function_for_side(direction);
		move_and_count_down_or_until_end(steps, state_function);
	}


	void move_and_reset()
	{
		//TODO:
	}


	inline void move_discretely(Event::Event& event)
	{
		CurtainState direction = event.direction();
		set_direction(direction);

		uint32_t movement_steps = steps(event);
		register uint32_t remaining_steps = move_and_count_down_or_until_end(movement_steps, direction);
		// The only way there are any steps remaining is if an endstop is triggered.
		if(remaining_steps > Config::Curtain::POSITION_TOLLERANCE)
		{
			// Because curtain open position is at the length of the curtain, the position will be 1 * length * the
			//  existance of OPEN_ENDSTOP (to take into account even if the pin mistakenly reads high). If the
			//  curtain is not open, then it is closed, because a endstop would have to be triggered to be here (or
			//  all steps would be consumed). 
			Global::curtain.position(Global::curtain.length() * is_open() * OPEN_ENDSTOP);
			if(Global::curtain.auto_correct())
			{
				step(event);
				Global::curtain.percentage(event.percentage());  // call it good enough
			}
		}
		else
		{
			Global::curtain.percentage(event.percentage());  // alleged success
		}
	}


	// For functionality, see: Movement::move_until_state_reached(.).
	// Takes movement direction flag option (that is then XOR-ed).
	inline void move_until_closed()
	{
		set_direction(CLOSE);
		move_until_state_reached(is_closed);
	}


	// For functionality, see: move_until_state_reached(.).
	// Takes movement direction flag option (that is then XOR-ed).
	inline void move_until_open()
	{
		set_direction(OPEN);
		move_until_state_reached(is_open);
	}

	// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
	// Moves to an end until sensor is tripped.
	// Takes a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
	// Does two pulses every iteration until sensor is tripped.
	inline void move_until_state_reached(bool(*state_function)())
	{
		enable_motor();

		while(!state_function())
		{
			pulse_twice();
		}

		disable_motor();
	}


	// ——————————————————————————————————————————————— MOVEMENT::STEP ——————————————————————————————————————————————— //
	// Step implies a reliant on the counting of steps to know position & is unguarded by hardware.

	inline void step(Event::Event& event)
	{
		step(steps(event), event.direction());
	}


	inline void step(register uint32_t steps)
	{
		// make number of steps an even amount to match movement loop (prevent overflow)
		steps &= 0xFFFFFFFE;
		while(steps)
		{
			pulse_twice();
			steps -= 2;
		}
		disable_motor();
	}


	inline void step(register uint32_t steps, const bool direction)
	{
		set_direction(direction);
		enable_motor();
		step(steps);
	}


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


}