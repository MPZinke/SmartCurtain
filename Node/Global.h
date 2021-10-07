
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Stores info that is included on other dependencies of the Node.ino file.  Breaks values into differnt *
*    namespaces to separate functionalities. Holds C-String custom functions (similar to classic C-String functions),  *
*    Curtain class declaration & other global stuff, Json functions for received message interpretation. JSON          *
*    functions are limited to JSON formats received for this project.                                                  *
*   NOTES:   - With uint8_t return values, 255 (-1) is often used as a "bad value". Therefore, iterations must stop at *
*              less than 255.  Otherwise, the 255th value and -1 will be ambiguous or the final x++ will overflow the  *
*              returned number or a bad number is returned.                                                            *
*   BUGS:                                                                                                              *
*   FUTURE:  - Consider expanding JSON functions to be less exclusive                                                  *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include <assert.h>
#include <HttpClient.h>

#include "User.h"
// Defined in User.h
#if __ETHERNET__
	#include <Ethernet.h>
	#include <EthernetClient.h>
#elif __WIFI__
	#include <WiFi.h>
	#include <WiFiClient.h>
	#include <esp_wifi.h>
#endif


#define JSON_BUFFER_SIZE 255


// ————————————————————————————————————————————————————— C-STRING —————————————————————————————————————————————————————

namespace C_String
{
	// ——————————————————————————————————————————————— C-STRING: GLOBAL ———————————————————————————————————————————————

	void copy(const char[], char[]);
	void copy_n(const char[], char[], uint8_t);
	void itoa(uint32_t, char[]);
	uint8_t length(char[]);


	// —————————————————————————————————————————————— C-STRING: FUNCTIONS ——————————————————————————————————————————————

	// Copies one c string to another & null terminates.
	// Takes address of place to read from, address of place to write to.
	// Iterates over number of character reading then writing.  Null terminates "to" after 254 or Null found.
	void copy(const char from[], char to[])
	{
		uint8_t x;
		for(x = 0; x < 255 && from[x]; x++) to[x] = from[x];
		to[x] = 0;
	}


	// Copies n number of character & null terminates.
	// Takes address of place to read from, address of place to write to, number of character to write.
	// Iterates over number of character reading then writing.  Null terminates "to" after n-chars written.
	void copy_n(const char from[], char to[], uint8_t length)
	{
		for(uint8_t x = 0; x < length; x++) to[x] = from[x];
		to[length] = 0;
	}


	// Converts an int to a c-string.
	// Takes the integer to be converted, the address of the location to convert it to.
	// Converts from least signicicant bit first, then reverses chars.
	void itoa(uint32_t integer, char to[])
	{
		if(!integer)
		{
			*to = 48;
			to[1] = 0;
			return;
		}

		uint8_t x;
		for(x = 0; x < 255 && integer; x++)
		{
			to[x] = (integer % 10) + 48;  // add character
			integer /= 10;
		}
		to[x--] = 0;  // null terminate and back step

		// the old switch-a-roo: switch character order since it was written backwards
		for(register uint8_t left_index = 0; left_index < (x+1) / 2; left_index++)
		{
			register uint8_t right_index = x - left_index;
			to[left_index] ^= to[right_index];
			to[right_index] ^= to[left_index];
			to[left_index] ^= to[right_index];
		}
	}


	// LENGTH DOES NOT INCLUDE Null terminator.
	// The old tried and test string with the new twist of a better name. ;)
	// Takes a byte array (that is hopefully Null Terminated).
	// Iterates array until Null terminator is found or max length is reached.
	// Return length of string (or max uint8_t).
	uint8_t length(char string[])
	{
		uint8_t length = 255;
		while(length && string[255-length]) length--;
		return 255-length;
	}

} // end namespace C_String


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


	// declared here for Gpio.h functions, since Gpio.h is called in Curtain.h (and thus exists before it)
	class Curtain
	{
		private:
			// ———— OPTIONS ————
			bool _auto_calibrate;  // if the curtain has opportunity to move full span, count steps & return value
			bool _auto_correct;  // if position is unexpected, go to expected position
			bool _direction;  // XOR for direction (to switch which way is open)
			bool _is_smart;  // whether the hardware is able to figure out where it is
			// ———— CURRENT DATA ON CURTAIN ————
			uint32_t _current_position;  // the current length according to the RPi
			uint32_t _length;  // overall length of the curtain [steps]
			// ———— EVENT INFO ————
			uint32_t _desired_position;  // desired position according to the curtain
			uint32_t _event;  // CurtainsEvents.id (0 if no event)

		public:
			Curtain(StaticJsonDocument<JSON_BUFFER_SIZE>&);
			char* serialize_data();

			// —————————————— GETTERS: ATTRIBUTES ——————————————
			bool calibrate();
			bool correct();
			bool direction();
			bool is_smart();

			uint32_t current_position();
			uint32_t length();

			uint32_t desired_position();
			uint32_t event();

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

			// —————————————— WRITE ——————————————
			void send_hub_serialized_info();
	};

} // end namespace Curtain


// —————————————————————————————————————————————————————— GLOBAL ——————————————————————————————————————————————————————

namespace Global
{
	const uint32_t ignore_movement_similarity = 10;  // max step difference to ignore event
	const uint32_t wiggle_room = 5;  // steps within ends to consider "end zones"
	const uint32_t steps_for_calibration = 5;  // how picky the program should be movement

	const uint16_t loop_wait = 1024;  // a nice power of 2

#if __ETHERNET__
	EthernetServer server(User::port);
#elif __WIFI__
	WiFiServer server(User::port);
#endif
	HARDWARE_CLIENT client;

} // end namespace Global
