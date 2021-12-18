
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


#pragma once


#include "Global.hpp"

#include "Exceptions.hpp"
#include "Movement.hpp"
#include "Transmission.hpp"


namespace Curtain
{

	using Movement::CurtainState;


	bool is_approximate_position(uint32_t, uint32_t);
	bool is_approximate_position(uint32_t, uint32_t, uint32_t);
	CurtainState approximate_state_of(uint32_t, uint32_t);
	CurtainState state_of(uint32_t, uint32_t);


	// —————————————————————————————————————————————————— CURTAIN —————————————————————————————————————————————————— //


	class Event
	{
		private:
			uint32_t _id;
			uint32_t _curtain_length;
			bool _force;
			uint32_t _position;

		public:
			Event(uint32_t id, uint32_t curtain_length, bool force, uint32_t desired_position);

			uint32_t id();
			uint32_t position();

			// ———— MOVEMENT ———— //
			bool event_moves_to_an_end();
			bool moves_full_span();
			CurtainState state_of_position();
	};


	// declared here for Gpio.h functions, since Gpio.h is called in Curtain.h (and thus exists before it)
	class Curtain
	{
		private:
			// ———— OPTIONS ————
			bool _auto_calibrate;  // if the curtain has opportunity to move full span, count steps & return value
			bool _auto_correct;  // if position is unexpected, go to expected position
			bool _discrete_movement;  // if the curtain can move to a discrete location (not just open or closed)
			// ———— CURRENT DATA ON CURTAIN ————
			uint32_t _position;  // the current length according to the RPi
			uint32_t _length;  // overall length of the curtain [steps]
			// ———— EVENT INFO ————
			Event _event;

		public:
			Curtain(StaticJsonDocument<Global::JSON_BUFFER_SIZE>&);
			char* serialize_data();

			// —————————————— GETTERS: ATTRIBUTES ——————————————
			bool calibrate();
			bool correct();
			bool move_discretely();

			uint32_t position();
			uint32_t length();
			Event event();

			// —————————————— GETTERS: DATA ——————————————
			bool should_calibrate_across();
			CurtainState state_of_position();

			// —————————————— SETTERS: ATTRIBUTES ——————————————
			void position(uint32_t new_position);
			void length(uint32_t new_length);

			// —————————————— SETTERS: DATA ——————————————
			void set_position_if_does_not_match_sensors();
			void set_location();

			// —————————————— WRITE ——————————————
			void send_hub_serialized_info();
	};

} // end namespace Curtain
