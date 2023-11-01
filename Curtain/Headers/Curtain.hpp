
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


template<size_t S>
class StaticString;


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
			const char* _id = Config::Curtain::CURTAIN_ID;
			const char _room_id[24+1] = "";
			const char _home_id[24+1] = "";

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
			const char* id() const;
			const char* room_id() const;
			const char* home_id() const;

			// —————————————————————————————————————————— GETTERS::OPTIONS —————————————————————————————————————————— //
			bool auto_correct() const;

			// ————————————————————————————————————————— GETTERS::HARDWARE  ————————————————————————————————————————— //
			uint32_t length() const;
			bool direction() const;
			bool is_moving() const;
			uint32_t percentage() const;

			// ——————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————— //
			operator StaticString<JSON_BUFFER_SIZE>() const;

			// —————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————— //
			// —————————————————————————————————————————————————————————————————————————————————————————————————————— //

			// ————————————————————————————————————————— SETTERS::STRUCTURE ————————————————————————————————————————— //
			void room_id(const char* new_room_id);
			void home_id(const char* new_home_id);

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

	bool validate(DeserializedJSON::DeserializedJSON& curtain_json);
}
