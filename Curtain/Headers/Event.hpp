
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


#include <Arduino.h>
#include <ArduinoJson.h>


#include "Config.hpp"
#include "Movement.hpp"


namespace DeserializedJSON
{
	class DeserializedJSON;
}


template<size_t S>
class StaticString;


namespace Event
{
	using Movement::CurtainState;


	class Event
	{
		private:
			uint8_t _percentage;

		public:
			Event(DeserializedJSON::DeserializedJSON& event_json);

			operator StaticString<JSON_BUFFER_SIZE>();

			// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
			uint8_t percentage();

			// —————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————— //
			void percentage(uint8_t percentage);

			// —————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————— //
			CurtainState direction();  // The direction the curtain will move towards
			bool event_moves_to_an_end();
			CurtainState state();  // The final state of the curtain
			uint32_t steps();
	};

	bool validate(DeserializedJSON::DeserializedJSON& event_json);
}
