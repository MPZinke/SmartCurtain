
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.19                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Event.hpp"

#include "../Headers/Transmission.hpp"


namespace Event
{
	using namespace Movement::CurtainStates;


	Event::Event(JsonObject& event_object)
	{
		_id = event_object[Transmission::Literal::JSON::Key::EVENT_ID];
		_position = event_object[Transmission::Literal::JSON::Key::EVENT_POSITION];
		_force = false;
		if(event_object.containsKey(Transmission::Literal::JSON::Key::EVENT_FORCE))
		{
			_force = event_object[Transmission::Literal::JSON::Key::EVENT_FORCE];
		}
	}


	Event::Event(uint32_t id, bool force, uint32_t position)
	{
		_id = id;
		_force = force;
		_position = position;
	}


	uint32_t Event::id()
	{
		return _id;
	}


	uint32_t Event::position()
	{
		return _position;
	}


	bool Event::event_moves_to_an_end()
	{
		return _position == 0 || _position == Global::curtain.length();
	}


	// Determines whether the curtain moves all the way across the rod (open to close) for desired position.
	// Get the state of the curtain based of GPIO.  Compares with the state of the desired position.
	// Returns true if curtain moves all the way across rod, false otherwise.
	inline bool Event::moves_full_span()
	{
		CurtainState curtian_state = Movement::current_state();
		CurtainState desired_state = Movement::state_of(_position);
		// parens not needed (precedence) but used to remove warnings
		return (curtian_state == CLOSED && desired_state == OPEN) || (curtian_state == OPEN && desired_state == CLOSED);
	}


	// SUGAR: whether desired position is open/close/middle.
	CurtainState Event::state()
	{
		return Movement::state_of(_position);
	}
}  // end namespace Event
