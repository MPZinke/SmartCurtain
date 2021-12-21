
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


#include "../Headers/Movement.hpp"


namespace Event
{
	using Movement::CurtainState;


	class Event
	{
		private:
			uint32_t _id;
			uint8_t _percentage;

		public:
			Event(JsonObject& event_object);
			Event(uint32_t id, uint8_t desired_percentage);

			operator char*();
			operator JsonObject();

			// ———— GETTERS ———— //
			uint32_t id();
			uint8_t percentage();

			// ———— MOVEMENT ———— //
			CurtainState direction();  // The direction the curtain will move towards
			bool event_moves_to_an_end();
			bool moves_full_span();  // Whether the event causes the curtain to move full span
			CurtainState state();  // The final state of the curtain
	};
}
