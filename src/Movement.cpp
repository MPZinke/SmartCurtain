
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
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
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
		if(Global::curtain.is_moving() == true)
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
			else if(event.direction() == CLOSE && Global::curtain.auto_correct())
			{
				if(!Secure::move_and_count_down_or_until_closed(event.steps()))
				{	
					Global::curtain.update();  // ensure curtain is up to date with hardware
					Hardware::set_direction(OPEN);
					Unsecure::step(event.steps());
				}
			}
			else
			{
				Hardware::set_direction(event.direction());
				Unsecure::step(event.steps());
			}
		}

		Global::curtain.is_moving(false);
		Message::update_hub();
	}


	// —————————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	namespace Secure
	{
		uint32_t move_and_count_to_closed()
		/*
		SUMMARY: 
		PARAMS:  
		DETAILS: 
		RETURNS: 
		*/
		{
			Hardware::set_direction(CLOSED);
			uint32_t steps_not_taken;
			for(steps_not_taken = 0xFFFFFFFFFFFFFFFF; steps_not_taken != 0 && !Hardware::is_closed(); steps_not_taken--)
			{
				Hardware::pulse();
			}

			return 0xFFFFFFFFFFFFFFFF - steps_not_taken;
		}


		// DESTRUCTIVE IF USED INCORRECTLY (IE WRONG END STOP FUNCTION: is_open/is_closed).
		// Checks whether an endstop has been reached after taking each step.
		// Takes number of steps, a function pointer (is_open/is_closed) used to determine whether the sensor is tripped.
		// Does two pulses every iteration until all steps take or sensor is tripped.
		// Returns remaining steps.
		uint32_t move_and_count_down_or_until_closed(register uint32_t remaining_steps)
		/*
		SUMMARY: 
		PARAMS:  
		DETAILS: 
		RETURNS: 
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


		// For functionality, see: Movement::move_until_state_reached(.).
		// Takes movement direction flag option (that is then XOR-ed).
		inline void move_until_closed()
		/*
		SUMMARY: 
		PARAMS:  
		DETAILS: 
		RETURNS: 
		*/
		{
			Hardware::set_direction(CLOSE);
			Hardware::enable_motor();

			for(uint32_t x = 0xFFFFFFFFFFFFFFFF; 0 < x && !Hardware::is_closed(); x--)
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
		SUMMARY: 
		PARAMS:  
		DETAILS: 
		RETURNS: 
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
