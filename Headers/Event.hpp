
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
			uint8_t _percentage;

		public:
			Event(String& event_json);

			operator String();

			// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
			uint8_t percentage();

			// —————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————— //
			CurtainState direction();  // The direction the curtain will move towards
			bool event_moves_to_an_end();
			bool moves_full_span();  // Whether the event causes the curtain to move full span
			CurtainState state();  // The final state of the curtain
	};
}
