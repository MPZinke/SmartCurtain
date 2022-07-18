
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
			uint32_t _id;
			bool _is_finished = false;
			uint8_t _percentage;

		public:
			Event(JsonObject& event_object);
			Event(uint32_t id, uint8_t percentage, bool is_finished=false);

			operator String();

			// ———— GETTERS ———— //
			uint32_t id();
			bool is_finished();
			uint8_t percentage();

			// ———— SETTERS ———— //
			void is_finished(bool new_is_finished);

			// ———— MOVEMENT ———— //
			CurtainState direction();  // The direction the curtain will move towards
			bool event_moves_to_an_end();
			bool moves_full_span();  // Whether the event causes the curtain to move full span
			CurtainState state();  // The final state of the curtain

			// ———— OTHER ———— //
			void append_to(JsonObject& json_object);
	};
}
