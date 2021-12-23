
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
			// ———— OPTIONS ———— //
			bool _auto_calibrate;  // if the curtain has opportunity to move full span, count steps & return value
			bool _auto_correct;  // if position is unexpected, go to expected position
			bool _direction;
			bool _discrete_movement;  // if the curtain can move to a discrete location (not just open or closed)
			uint32_t _length;  // overall length of the curtain [steps]
			uint8_t _percentage;  // the percentage amount open of the curtain
			uint32_t _position;  // the current length according to the RPi

			uint32_t _last_event_id = 0;  // the ID of the last event (helps determine if fresh restart)

		public:
			Curtain(bool initialize);

			operator char*();
			operator JsonObject();

			// ———— GETTERS ————
			bool auto_calibrate();
			bool auto_correct();
			bool direction();
			bool discrete_movement();
			uint32_t length();
			uint8_t percentage();
			uint32_t position();
			CurtainState state();

			// ———— SETTERS ————
			void auto_calibrate(register bool new_auto_calibrate);
			void auto_correct(register bool new_auto_correct);
			void direction(register bool new_direction);
			void length(register uint32_t new_length);
			void discrete_movement(register bool new_discrete_movement);
			void percentage(register uint8_t new_percentage);
			void position(register uint32_t new_position);
			void update(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);

			// —————————————— SETTERS: DATA ——————————————
			// void set_position_if_does_not_match_sensors();
			// void set_location();

			// —————————————— WRITE ——————————————
			// void send_hub_serialized_info();
	};

} // end namespace Curtain


#endif
