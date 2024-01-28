
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
#include "StaticString.hpp"


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
		friend class Event::Event;

		private:
			// ————————————————————————————————————————————— STRUCTURE  ————————————————————————————————————————————— //
			const char* _name = Config::Curtain::NAME;
			const char* _room = Config::Curtain::ROOM;

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
			// ————————————————————————————————————————— SETTERS::HARDWARE  ————————————————————————————————————————— //
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
