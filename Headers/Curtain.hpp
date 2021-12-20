
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


#ifndef __CURTAIN__
#define __CURTAIN__


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
			// ———— OPTIONS ————
			bool _auto_calibrate;  // if the curtain has opportunity to move full span, count steps & return value
			bool _auto_correct;  // if position is unexpected, go to expected position
			bool _discrete_movement;  // if the curtain can move to a discrete location (not just open or closed)
			// ———— CURRENT DATA ON CURTAIN ————
			bool _direction;
			uint32_t _length;  // overall length of the curtain [steps]
			uint32_t _position;  // the current length according to the RPi

		public:
			Curtain(bool initialize);

			// —————————————— GETTERS: ATTRIBUTES ——————————————
			bool auto_calibrate();
			bool auto_correct();
			bool direction();
			bool discrete_movement();
			uint32_t length();
			uint32_t position();

			// —————————————— GETTERS: DATA ——————————————
			bool should_calibrate_across();
			CurtainState state_of_position();

			// —————————————— SETTERS: ATTRIBUTES ——————————————
			void auto_calibrate(bool new_auto_calibrate);
			void auto_correct(bool new_auto_correct);
			void direction(bool new_direction);
			void length(uint32_t new_length);
			void discrete_movement(bool new_discrete_movement);
			void position(uint32_t new_position);
			void update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);

			operator char*();
			operator JsonObject();

			// —————————————— SETTERS: DATA ——————————————
			// void set_position_if_does_not_match_sensors();
			// void set_location();

			// —————————————— WRITE ——————————————
			// void send_hub_serialized_info();
	};

} // end namespace Curtain


#endif
