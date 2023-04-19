
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

	// ——————————————————————————————————————————————— CURTAIN OBJECT ——————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// declared here for Gpio.h functions, since Gpio.h is called in Curtain.h (and thus exists before it)
	class Curtain
	{
		private:
			// ————————————————————————————————————————————— STRUCTURE  ————————————————————————————————————————————— //
			const uint16_t _id;
			uint16_t _home_id = 0;
			uint16_t _room_id = 0;

			// ———————————————————————————————————————————— INFORMATION  ———————————————————————————————————————————— //
			uint32_t _length = Config::Hardware::MAX_LENGTH;  // overall length of the curtain [steps]
			uint8_t _percentage = 0;  // the percentage amount open of the curtain
			uint32_t _position = 0;  // the current length according to the RPi

			// —————————————————————————————————————————————— OPTIONS  —————————————————————————————————————————————— //
			bool _auto_calibrate = Config::Hardware::BOTH_ENDSTOPS;  // count steps & return value, if moves full span
			bool _auto_correct = Config::Hardware::EITHER_ENDSTOP;  // go to expected position, if unexpected position
			bool _direction = Config::Hardware::DIRECTION_SWITCH;  // flip what value goes what direction
			// if the curtain can move to a discrete location (not just open or closed)
			bool _discrete_movement = Config::Hardware::EITHER_ENDSTOP && Config::Hardware::DISCRETE_MOVEMENT_ALLOWED;

		public:
			Curtain(uint8_t id);

			// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
			// —————————————————————————————————————————————————————————————————————————————————————————————————————— //

			// ————————————————————————————————————————— GETTERS::STRUCTURE ————————————————————————————————————————— //
			uint8_t id();
			uint8_t home_id();
			uint8_t room_id();
			// ———————————————————————————————————————— SETTERS::INFORMATION ———————————————————————————————————————— //
			uint32_t length();
			uint8_t percentage();
			uint32_t position();
			// —————————————————————————————————————————— GETTERS::OPTIONS —————————————————————————————————————————— //
			bool auto_calibrate();
			bool auto_correct();
			bool direction();
			bool discrete_movement();
			// ——————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————— //
			CurtainState state();
			operator String();

			// —————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————— //
			// —————————————————————————————————————————————————————————————————————————————————————————————————————— //

			// ————————————————————————————————————————— SETTERS::STRUCTURE ————————————————————————————————————————— //
			void home_id(register uint16_t new_home_id);
			void room_id(register uint16_t new_room_id);
			// ———————————————————————————————————————— SETTERS::INFORMATION ———————————————————————————————————————— //
			void length(register uint32_t new_length);
			void percentage(register uint8_t new_percentage);
			void position(register uint32_t new_position);
			// —————————————————————————————————————————— SETTERS::OPTIONS —————————————————————————————————————————— //
			void auto_calibrate(register bool new_auto_calibrate);
			void auto_correct(register bool new_auto_correct);
			void direction(register bool new_direction);
			void discrete_movement(register bool new_discrete_movement);
			// ——————————————————————————————————————————— SETTERS::OTHER ——————————————————————————————————————————— //
			void update();

			// ———— OTHER ———— //
			void append_to(JsonObject& json_object);

		private:
			void update_from_state();
	};
} // end namespace Curtain
