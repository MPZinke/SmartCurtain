
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


#include "Movement.hpp"


namespace DeserializedJSON
{
	class DeserializedJSON;
}


namespace Event
{
	using Movement::CurtainState;


	class Event
	{
		private:
			uint8_t _percentage;

		public:
			Event(DeserializedJSON::DeserializedJSON& event_json);

			operator String();

			// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
			uint8_t percentage();

			// —————————————————————————————————————————————— MOVEMENT —————————————————————————————————————————————— //
			CurtainState direction();  // The direction the curtain will move towards
			bool event_moves_to_an_end();
			CurtainState state();  // The final state of the curtain
			uint32_t steps();
	};


	inline String invalid_key_message(const char* key, const char* type_str);
	bool validate(DeserializedJSON::DeserializedJSON& event_json);
}
