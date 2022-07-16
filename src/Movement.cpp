
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

#include "../Headers/Config.hpp"
#include "../Headers/Curtain.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/Hardware.hpp"


namespace Movement
{
	// Import values from Hardware
	using Config::Hardware::BOTH_ENDSTOPS;
	using Config::Hardware::CLOSE_ENDSTOP;
	using Config::Hardware::OPEN_ENDSTOP;
	using namespace Hardware::CurtainStates;  // used CurtainStates as enum


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


	// SUMMARY: Gets the precises state of the hardware or the approximate state of the curtain.
	// DETAILS: Checks available hardware for their state. If the hardware is unable to precisely determine state, it
	//  gets the state based on Global::Curtain details.
	// RETURNS: The CurtainState value of the state.
	CurtainState current_state()
	{
		// Attempt to get state through hardware.
		register CurtainState curtain_state;
		if((curtain_state = Hardware::current_hardware_state()) != UNKNOWN)
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

	bool (*function_for_side(CurtainState state))()
	{
		bool(*direction_function[2])();
		direction_function[CLOSE] = Hardware::is_closed;
		direction_function[OPEN] = Hardware::is_open;

		return direction_function[state];
	}


	bool return_false()
	{
		return false;
	}


	bool return_true()
	{
		return true;
	}


	uint32_t steps()
	{
		uint32_t desired_position = (uint32_t)Global::event.percentage() * Global::curtain.length() / 100;

		// More complicated statements are processed in if clause so that if the pipeline is broken, there is less to
		//  recover to.
		if(Global::event.direction() == OPEN)
		{
			return desired_position - Global::curtain.position();
		}
		return Global::curtain.position();
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// SUMMARY: Activates curtain for an event to move to a certain position.
	// PARAMS:  Takes the reference to the event.
	// DETAILS: Determines the movement time based on hardware if hardware exists.
	void activate()
	{
		if(Global::curtain.auto_calibrate() && event.moves_full_span())
		{
			Secure::move_and_calibrate(event);
		}
		// If hardware allowed and the event moves to CLOSED (including non-discrete only movement)
		else if(CLOSE_ENDSTOP && Global::event.state() == CLOSED)
		{
			Secure::move_until_closed();
		}
		// If hardware allowed and the event moves to OPEN (including non-discrete only movement)
		else if(OPEN_ENDSTOP && Global::event.state() == OPEN)
		{
			Secure::move_until_open();
		}
		// Non-discrete, movement is covered by Event object. If the movement is discrete, it will automatically go to
		//  the else of the below if, since it is automatically converted to OPEN/CLOSED. If it had an endstop and were
		//  supposed to move to an end, the event would be caught above.
		// if statement: has ability to prevent self-destructive behavior
		else if((OPEN_ENDSTOP && Global::event.direction() == OPEN)
		  || (CLOSE_ENDSTOP && Global::event.direction() == CLOSE))
		{
			Secure::move_discretely();
		}
		// —— No endstops below this point ——
		// —— Position is assumed below this point (even with an encoder) ——
		// REMINDER: For non-discrete: Percentage is converted to 0 or 100 and movement is assumed to be correct.
		// REMINDER: For discrete: Percentage remains the same and movement is assumed to be correct.
		// Because event percentage is automatically changed if the curtain is non-discrete and current position is
		//  only able to be assumed, discrete and non-discrete movements work the same at this point. Curtain can just
		//  be told to step, regardless of if it is discretely between or the full length of the curtain.
		else
		{
			Unsecure::step();
		}
	}


	namespace Secure
	{

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
			Hardware::enable_motor();

			register uint32_t steps = 0;
			while(!state_function())
			{
				Hardware::pulse_twice();
				steps += 2;
			}

			Hardware::disable_motor();
			return steps;
		}


		// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
		// Checks whether an endstop has been reached after taking each step.
		// Takes number of steps, a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
		// Does two pulses every iteration until all steps take or sensor is tripped.
		// Returns remaining steps.
		uint32_t move_and_count_down_or_until_end(register uint32_t steps, bool (*state_function)())
		{
			Hardware::enable_motor();

			steps = steps & 0xFFFFFFFE;  // make number of steps an even amount to match movement loop (prevent overflow)
			while(steps && !state_function())
			{
				Hardware::pulse_twice();
				steps -= 2;
			}

			Hardware::disable_motor();
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
			//TODO
		}


		inline void move_discretely()
		{
			CurtainState direction = Global::event.direction();
			Hardware::set_direction(direction);

			uint32_t movement_steps = steps();
			register uint32_t remaining_steps = move_and_count_down_or_until_end(movement_steps, direction);
			// The only way there are any steps remaining is if an endstop is triggered.
			if(remaining_steps > Config::Curtain::POSITION_TOLLERANCE)
			{
				// Because curtain open position is at the length of the curtain, the position will be 1 * length * the
				//  existance of OPEN_ENDSTOP (to take into account even if the pin mistakenly reads high). If the
				//  curtain is not open, then it is closed, because a endstop would have to be triggered to be here (or
				//  all steps would be consumed). 
				Global::curtain.position(Global::curtain.length() * Hardware::is_open() * OPEN_ENDSTOP);
				if(Global::curtain.auto_correct())
				{
					::Movement::Unsecure::step();
					Global::curtain.percentage(Global::event.percentage());  // call it good enough
				}
			}
			else
			{
				Global::curtain.percentage(Global::event.percentage());  // alleged success
			}
		}


		// For functionality, see: Movement::move_until_state_reached(.).
		// Takes movement direction flag option (that is then XOR-ed).
		inline void move_until_closed()
		{
			Hardware::set_direction(CLOSE);
			move_until_state_reached(Hardware::is_closed);
		}


		// For functionality, see: move_until_state_reached(.).
		// Takes movement direction flag option (that is then XOR-ed).
		inline void move_until_open()
		{
			Hardware::set_direction(OPEN);
			move_until_state_reached(Hardware::is_open);
		}

		// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
		// Moves to an end until sensor is tripped.
		// Takes a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
		// Does two pulses every iteration until sensor is tripped.
		inline void move_until_state_reached(bool(*state_function)())
		{
			Hardware::enable_motor();

			while(!state_function())
			{
				Hardware::pulse_twice();
			}

			Hardware::disable_motor();
		}

	}  // end namespace Movement::StateGuarded


	namespace Unsecure
	{

		// ——————————————————————————————————————————————— MOVEMENT::STEP ——————————————————————————————————————————————— //
		// Step implies a reliant on the counting of steps to know position & is unguarded by hardware.

		inline void step()
		{
			Hardware::set_direction(Global::event.direction());
			Hardware::enable_motor();
			// make number of steps an even amount to match movement loop (prevent overflow)
			register uint32_t remaining_steps = steps() & 0xFFFFFFFE;
			while(remaining_steps)
			{
				Hardware::pulse_twice();
				remaining_steps -= 2;
			}
			Hardware::disable_motor();
		}
	}  // end namespace Movement::Unsecure
}  // end namespace Movement
