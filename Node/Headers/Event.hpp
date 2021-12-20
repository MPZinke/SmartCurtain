
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
			uint32_t _curtain_length;
			bool _force;
			uint32_t _position;

		public:
			Event(JsonObject& event_object);
			Event(uint32_t id, bool force, uint32_t desired_position);

			uint32_t id();
			uint32_t position();

			// ———— MOVEMENT ———— //
			bool event_moves_to_an_end();
			bool moves_full_span();
			CurtainState state();
	};
}
