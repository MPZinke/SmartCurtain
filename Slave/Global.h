
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on ..                                                                                                              *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#ifndef _GLOBAL_
#define _GLOBAL_

#include "User.h"

// —————————————————————————————————————————————————————— CURTAIN ——————————————————————————————————————————————————————

namespace Curtain  // also exists in Curtain.h
{
	// ———————————————————————————————————————————————— CURTAIN: TYPES ————————————————————————————————————————————————

	typedef enum
	{
		CLOSED = -1,
		MIDDLE,
		OPEN
	} CurtainState;


	// here declare for GPIO.h functions, since GPIO.h is called in Curtain.h (and thus exists before it)
	class Curtain
	{
		private:
			// if the curtain has opportunity to move full span, count steps & return value
			// if position is unexpected, go to expected position
			// XOR for direction (to switch which way is open)
			byte _options;  // see above
			uint32_t _current_position;  // the current length according to the RPi
			uint32_t _desired_position;  // desired position according to the curtain
			uint32_t _length;  // overall length of the curtain [steps]

		public:
			Curtain(byte[]);
			void encode(byte[]);

			// —————————————— GETTERS: ATTRIBUTES ——————————————
			bool calibrate();
			bool correct();
			bool direction();

			uint32_t current_position();
			uint32_t desired_position();
			uint32_t length();

			// —————————————— GETTERS: DATA ——————————————
			bool event_moves_to_an_end();
			bool moves_full_span();
			bool should_calibrate_across();
			CurtainState state_of_current_position();
			CurtainState state_of_desired_position();

			// —————————————— SETTERS: ATTRIBUTES ——————————————
			void current_position(uint32_t);
			void desired_position(uint32_t);
			void length(uint32_t);

			// —————————————— SETTERS: DATA ——————————————
			void set_current_position_if_does_not_match_sensors();
			void set_location();
	};
}


// —————————————————————————————————————————————————————— GLOBAL ——————————————————————————————————————————————————————

namespace Global
{
	const uint32_t ignore_movement_similarity = 10;  // max step difference to ignore event
	const uint32_t wiggle_room = 5;  // steps within ends to consider "end zones"
	const uint32_t steps_for_calibration = 5;  // how picky the program should be movement


	IPAddress server(User::master_host[0], User::master_host[1], User::master_host[2], User::master_host[3]);
	EthernetClient client;  // the magician
}


// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

#define DEF(x) #x  // convert a define into a string

#endif
