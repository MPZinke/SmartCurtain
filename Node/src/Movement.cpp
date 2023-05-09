
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
#include "../Headers/Global.hpp"

#include "../Headers/Curtain.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Hardware.hpp"
#include "../Headers/Message.hpp"


namespace Movement
{
	// Import values from Hardware
	using namespace Hardware::CurtainStates;  // used CurtainStates as enum


	void reset()
	/*
	SUMMARY: Resets the curtain by moving it to closed and then back to its original position, counting steps along the
	         way.
	DETAILS: Moves the curtain to a closed position, counting steps along the way. Then moves that number of steps back
	         to the other direction.
	*/
	{
		if(Hardware::state() == CLOSED)
		{
			Global::curtain.percentage(0);
		}
		else
		{
			uint32_t steps = Secure::move_and_count_to_closed();
			Hardware::set_direction(OPEN);
			Unsecure::step(steps);
			uint8_t curtain_percentage = steps * 100 / Global::curtain.length();
			Global::curtain.percentage(curtain_percentage);
		}

		Global::curtain.is_moving(false);
		Message::update_hub();
	}


	void move(Event::Event event)
	/*
	NOTES: Expects that curtain._is_moving is true.
	*/
	{
		// Ensure that the is moving flag has been set so that the movement thread spawner can check whether the curtain
		//  is already moving. If the movement thread spawner sees the curtain is already moving, then it will not
		//  create another event. This prevents multiple resources trying to control the hardware at once.
		if(Global::curtain.is_moving() != true)
		{
			return;
		}

		Global::curtain.update();  // ensure curtain is up to date with hardware
		if(Global::curtain.percentage() != event.percentage())
		{
			if(event.percentage() == 0)
			{
				Secure::move_until_closed();
			}
			// Move towards a closed position
			else if(event.direction() == CLOSE)
			{
				// If failed to reach non-zero state without hitting 0 and allowed to correct self:
				if(!Secure::move_towards_closed(event.steps()))
				{
					if(!Global::curtain.auto_correct())
					{
						event.percentage(0);  // GENERIC: event.percentage(100 * (event.direction != CLOSE));
					}
					else
					{
						Global::curtain.update();  // ensure curtain is up to date with hardware
						Hardware::set_direction(OPEN);
						Unsecure::step(event.steps());
					}
				}
			}
			// Move to an open position
			else
			{
				Hardware::set_direction(OPEN);
				Unsecure::step(event.steps());
			}
		}

		Global::curtain.percentage(event.percentage());
		Global::curtain.is_moving(false);
		Message::update_hub();
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	namespace Secure
	{
		inline uint32_t move_and_count_to_closed()
		/*
		SUMMARY: Moves the curtain to the closed position (until the closed endstop is reached) and counts the number of
		         steps to get there.
		RETURNS: Returns the number of steps taken to get to the closed position.
		*/
		{
			Hardware::set_direction(CLOSED);
			Hardware::enable_motor();
			uint32_t steps_not_taken;
			for(steps_not_taken = 0xFFFFFFFF; steps_not_taken != 0 && !Hardware::is_closed(); steps_not_taken--)
			{
				Hardware::pulse();
			}

			Hardware::disable_motor();
			return 0xFFFFFFFF - steps_not_taken;
		}


		inline bool move_towards_closed(register uint32_t remaining_steps)
		/*
		SUMMARY: Moves the curtain towards the closed position and checks whether the endstop has been triggered.
		PARAMS:  The number of steps to move.
		DETAILS: Moves towards closed, counting each step. If the close endstop has been triggered, curtain will stop
		         moving.
		RETURNS: Whether the curtain was able to move to its desired position.
		*/
		{
			Hardware::set_direction(CLOSE);
			Hardware::enable_motor();

			for(; 0 < remaining_steps && !Hardware::is_closed(); remaining_steps--)
			{
				Hardware::pulse();
			}

			Hardware::disable_motor();
			return remaining_steps == 0;
		}


		inline void move_until_closed()
		/*
		SUMMARY: Moves the curtain until the close endstop is triggered.
		*/
		{
			Hardware::set_direction(CLOSE);
			Hardware::enable_motor();

			for(uint32_t x = 0xFFFFFFFF; 0 < x && !Hardware::is_closed(); x--)
			{
				Hardware::pulse();
			}

			Hardware::disable_motor();
		}
	}  // end namespace Movement::StateGuarded


	namespace Unsecure
	{

		// ——————————————————————————————————————————————— MOVEMENT::STEP ——————————————————————————————————————————————— //
		// Step implies a reliant on the counting of steps to know position & is unguarded by hardware.

		inline void step(register uint32_t steps)
		/*
		SUMMARY: Steps the curtain a provided number of steps.
		PARAMS:  Takes the number of steps to move.
		*/
		{
			Hardware::enable_motor();

			// make number of steps an even amount to match movement loop (prevent overflow)
			for(; steps != 0; steps--)
			{
				Hardware::pulse();
			}

			Hardware::disable_motor();
		}
	}  // end namespace Movement::Unsecure
}  // end namespace Movement
