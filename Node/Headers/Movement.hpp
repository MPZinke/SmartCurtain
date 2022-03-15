
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


#include <stdint.h>


#include "Hardware.hpp"


namespace Event
{
	class Event;  // Declare it so it knows it exists
}


namespace Movement
{
	// Create conceptual aliases
	using Hardware::CurtainState;
	namespace CurtainStates = Hardware::CurtainStates;  // Alias so that other parts can use it as conceptually movement


	inline bool is_approximate_position(register uint32_t position1, register uint32_t position2,
	  register uint32_t allowable_difference);
	inline bool is_approximate_position(register uint32_t position1, register uint32_t position2);
	inline CurtainState approximate_state_of(register uint32_t position, register uint32_t curtain_length);
	CurtainState current_state();
	inline CurtainState state_of(register uint32_t position, register uint32_t curtain_length);
	inline CurtainState state_of(register uint32_t position);
	CurtainState state_of(register uint8_t percentage);

	bool (*function_for_side(CurtainState state))();
	bool return_false();
	bool return_true();
	uint32_t steps(Event::Event& event);
	void activate(register Event::Event& event);

	namespace EndstopGuarded
	{
		void move_and_calibrate(Event::Event& event);
		void move_and_reset();
		inline void move_discretely(Event::Event& event);
		inline void move_until_closed();
		inline void move_until_open();
		inline void move_until_state_reached(bool(*state_function)());
		uint32_t move_and_count_to_position_or_end(bool(*state_function)());
		uint32_t move_and_count_down_or_until_end(register uint32_t steps, bool (*state_function)());
		uint32_t move_and_count_down_or_until_end(register uint32_t steps, CurtainState direction);
	}  // end namespace EndstopGuarded

	namespace Endstopless
	{
		inline void step(Event::Event& event);
		inline void step(register uint32_t steps);
		inline void step(register uint32_t steps, const bool direction);
	}  // end namespace Endstopless
}  // end namespace GPIO
