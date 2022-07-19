
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.28                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Main Curtain class for organizing received data into an object. Also contains abstracted curtain      *
*    related functions such as what state a position would be.                                                         *
*   BUGS:                                                                                                              *
*   FUTURE: - Add cool temperature, light, & thermostat data.                                                          *
*                                                                                                                      *
***********************************************************************************************************************/


#include <ArduinoJson.h>


#include "Config.hpp"
#include "Movement.hpp"


namespace Curtain
{
	using Movement::CurtainState;

	// —————————————————————————————————————————————————— CURTAIN —————————————————————————————————————————————————— //

	// declared here for Gpio.h functions, since Gpio.h is called in Curtain.h (and thus exists before it)
	class Curtain
	{
		private:
			const uint8_t _id;
			// ———— OPTIONS ———— //
			// if the curtain has opportunity to move full span, count steps & return value
			bool _auto_calibrate = Config::Hardware::BOTH_ENDSTOPS;
			// if position is unexpected, go to expected position
			bool _auto_correct = Config::Hardware::EITHER_ENDSTOP;
			bool _direction = Config::Hardware::DIRECTION_SWITCH;
			// if the curtain can move to a discrete location (not just open or closed)
			bool _discrete_movement = Config::Hardware::EITHER_ENDSTOP && Config::Hardware::DISCRETE_MOVEMENT_ALLOWED;
			uint32_t _length = Config::Hardware::DEFAULT_LENGTH;  // overall length of the curtain [steps]
			uint8_t _percentage = 0;  // the percentage amount open of the curtain
			uint32_t _position = 0;  // the current length according to the RPi

			uint32_t _last_event_id = 0;  // the ID of the last event (helps determine if fresh restart)

		public:
			Curtain(uint8_t id);

			operator String();

			// ———— GETTERS ———— //
			uint8_t id();

			bool auto_calibrate();
			bool auto_correct();
			bool direction();
			bool discrete_movement();
			uint32_t length();
			uint8_t percentage();
			uint32_t position();
			CurtainState state();

			// ———— SETTERS ———— //
			void auto_calibrate(register bool new_auto_calibrate);
			void auto_correct(register bool new_auto_correct);
			void direction(register bool new_direction);
			void length(register uint32_t new_length);
			void discrete_movement(register bool new_discrete_movement);
			void percentage(register uint8_t new_percentage);
			void position(register uint32_t new_position);
			void update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);

			// ———— OTHER ———— //
			void append_to(JsonObject& json_object);

		private:
			void update_from_state();
	};
} // end namespace Curtain
