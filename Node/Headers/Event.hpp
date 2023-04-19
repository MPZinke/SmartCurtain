
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


#include <ArduinoJson.h>


#include "Movement.hpp"


namespace Event
{
	using Movement::CurtainState;


	class Event
	{
		private:
			bool _is_activated = false;
			bool _is_moving = false;
			uint8_t _percentage;

		public:
			Event(String& event_json);

			operator String();

			// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
			bool is_activated();
			bool is_moving();
			uint8_t percentage();

			// —————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————— //
			void is_activated(bool new_is_activated);
			void is_moving(bool new_is_moving);

			// —————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————— //
			CurtainState direction();  // The direction the curtain will move towards
			bool event_moves_to_an_end();
			bool moves_full_span();  // Whether the event causes the curtain to move full span
			CurtainState state();  // The final state of the curtain
	};
}
