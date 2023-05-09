
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
	class Event;
}


namespace Movement
{
	// Create conceptual aliases
	using Hardware::CurtainState;
	namespace CurtainStates = Hardware::CurtainStates;  // Alias so that other parts can use it as conceptually movement


	void move(Event::Event event);
	void reset();

	namespace Secure
	{
		inline uint32_t move_and_count_to_closed();
		inline bool move_towards_closed(register uint32_t remaining_steps);
		inline void move_until_closed();
	}  // end namespace Secure

	namespace Unsecure
	{
		inline void step(register uint32_t steps);
	}  // end namespace Unsecure
}  // end namespace GPIO
