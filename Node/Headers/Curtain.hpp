
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.11.27                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include <Arduino.h>


#include "Config.hpp"
#include "Movement.hpp"


namespace DeserializedJSON
{
	class DeserializedJSON;
}


namespace Event
{
	class Event;
}


namespace Curtain
{
	using Movement::CurtainState;
	namespace CurtainStates = Hardware::CurtainStates;


	// ——————————————————————————————————————————————— CURTAIN OBJECT ——————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	class Curtain
	{
		private:
			friend class Event::Event;
			// ————————————————————————————————————————————— STRUCTURE  ————————————————————————————————————————————— //
			uint16_t _id = Config::Curtain::CURTAIN_ID;
			uint16_t _room_id = 0;
			uint16_t _home_id = 0;

			// —————————————————————————————————————————————— HARDWARE —————————————————————————————————————————————— //
			bool _direction = Config::Hardware::DIRECTION_SWITCH;
			bool _is_moving = false;
			uint32_t _length = Config::Hardware::MAX_LENGTH;
			uint32_t _position = 0;

			// —————————————————————————————————————————————— OPTIONS  —————————————————————————————————————————————— //
			bool _auto_correct = true;

		public:
			Curtain();

			// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
			// —————————————————————————————————————————————————————————————————————————————————————————————————————— //

			// ————————————————————————————————————————— GETTERS::STRUCTURE ————————————————————————————————————————— //
			uint16_t id();
			uint16_t room_id();
			uint16_t home_id();

			// —————————————————————————————————————————— GETTERS::OPTIONS —————————————————————————————————————————— //
			bool auto_correct();

			// ————————————————————————————————————————— GETTERS::HARDWARE  ————————————————————————————————————————— //
			uint32_t length();
			bool direction();
			bool is_moving();
			uint32_t percentage();

			// ——————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————— //
			// CurtainState state();
			operator String();

			// —————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————— //
			// —————————————————————————————————————————————————————————————————————————————————————————————————————— //

			// ————————————————————————————————————————— SETTERS::STRUCTURE ————————————————————————————————————————— //
			void room_id(uint16_t new_room_id);
			void home_id(uint16_t new_home_id);

			// ————————————————————————————————————————— SETTERS::HARDWARE  ————————————————————————————————————————— //
			void direction(bool new_direction);
			void length(uint32_t new_length);
			void is_moving(bool new_is_moving);
			void percentage(uint32_t new_percentage);
			void update();

			// —————————————————————————————————————————— SETTERS::OPTIONS —————————————————————————————————————————— //
			void auto_correct(bool new_auto_correct);

			// ——————————————————————————————————————————————— OTHER  ——————————————————————————————————————————————— //
			// —————————————————————————————————————————————————————————————————————————————————————————————————————— //
			void operator=(DeserializedJSON::DeserializedJSON& curtain_json);
	};




	inline String invalid_key_message(const char* key, const char* type_str);
	bool validate(DeserializedJSON::DeserializedJSON& curtain_json);
}
