
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


#pragma once


#include <ArduinoJson.h>


#include "DeserializedJSON.hpp"
#include "Movement.hpp"


namespace Event
{
	using Movement::CurtainState;


	class Event
	{
		private:
			uint8_t _percentage;

		public:
			Event(DeserializedJSON& event_json);

			operator String();

			// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
			uint8_t percentage();
			static bool validate(DeserializedJSON& event_json);

			// —————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————— //
			CurtainState direction();  // The direction the curtain will move towards
			bool event_moves_to_an_end();
			CurtainState state();  // The final state of the curtain
			uint32_t steps();
	};


	bool validate(DeserializedJSON& event_json);
}
