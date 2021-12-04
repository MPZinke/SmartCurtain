
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
#include <exception>
#include <HttpClient.h>

#include "Configure.h"


#if __ETHERNET__
	#define __WIFI__ false

	#include <Ethernet.h>
	#include <EthernetClient.h>
#else
	#define __WIFI__ true

	#include <WiFi.h>
	#include <WiFiClient.h>
	#include <esp_wifi.h>
#endif


// —————————————————————————————————————————————————— PREPROCESSOR —————————————————————————————————————————————————— //


// ———— SMARTINESS ————
#if CLOSE_ENDSTOP
	#define IS_SMART true
	#define AUTO_CORRECT_CLOSE true
#endif

#if ENCODER || OPEN_ENDSTOP
	#define AUTO_CORRECT true
#endif

#if OPEN_ENDSTOP
	#define AUTO_CALIBRATE true
#endif



// DEFINITION::BUFFERS


// —————————————————————————————————————————————————————— GLOBAL ——————————————————————————————————————————————————————

namespace Global
{
	WiFiServer server(Config::Network::PORT);

	WiFiClient client;
	uint32_t current_position = 0;  // holds current position
	bool is_engaged = false;  // whether the motor is engaged (yay race conditions)

	const uint16_t JSON_BUFFER_SIZE = 0x1000;

} // end namespace Global



// ————————————————————————————————————————————————————— C-STRING —————————————————————————————————————————————————————

namespace C_String
{
	// ——————————————————————————————————————————————— C-STRING: GLOBAL ———————————————————————————————————————————————

	void copy(const char[], char[]);
	void copy_n(const char[], char[], uint16_t);
	void itoa(uint32_t, char[]);
	uint16_t length(char[]);


	// ———————————————————————————————————————————— C-STRING: FUNCTIONS ———————————————————————————————————————————— //

	uint32_t atoi(char string[])
	{
		uint16_t length = C_String::length(string);
		register uint32_t integer = 0;
		for(uint8_t x = 0; x < length; x++)
		{
			integer *= 10;
			integer += string[x] - 48;
		}

		return integer;
	}


	// Copies one c string to another & null terminates.
	// Takes address of place to read from, address of place to write to.
	// Iterates over number of character reading then writing.  Null terminates "to" after 254 or Null found.
	void copy(const char from[], char to[])
	{
		register uint16_t x;
		for(x = 0; x < Global::JSON_BUFFER_SIZE && from[x]; x++) to[x] = from[x];
		to[x] = 0;
	}


	// Copies n number of character & null terminates.
	// Takes address of place to read from, address of place to write to, number of character to write.
	// Iterates over number of character reading then writing.  Null terminates "to" after n-chars written.
	void copy_n(const char from[], char to[], uint16_t length)
	{
		for(register uint16_t x = 0; x < length; x++) to[x] = from[x];
		to[length] = 0;
	}


	bool equal(const char a[], char b[])
	{
		register uint16_t x;
		for(x = 0; x < 0xFFFF && a[x]; x++)
		{
			if(a[x] != b[x]) return false;
		}

		return a[x] == b[x];
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

		register uint16_t x;
		// OxFFFE because following for loop adds 1 (this prevents overflow)
		for(x = 0; x < 0xFFFE && integer; x++)
		{
			to[x] = (integer % 10) + 48;  // add character
			integer /= 10;
		}
		to[x--] = 0;  // null terminate and back step

		// the old switch-a-roo: switch character order since it was written backwards
		for(register uint16_t left_index = 0; left_index < (x+1) / 2; left_index++)
		{
			register uint16_t right_index = x - left_index;
			to[left_index] ^= to[right_index];
			to[right_index] ^= to[left_index];
			to[left_index] ^= to[right_index];
		}
	}


	// LENGTH DOES NOT INCLUDE Null terminator.
	// The old tried and test string with the new twist of a better name. ;)
	// Takes a byte array (that is hopefully Null Terminated).
	// Iterates array until Null terminator is found or max length is reached.
	// Return length of string (or max uint16_t).
	uint16_t length(char string[])
	{
		uint16_t length = Global::JSON_BUFFER_SIZE;
		while(length && string[Global::JSON_BUFFER_SIZE-length]) length--;
		return Global::JSON_BUFFER_SIZE-length;
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


	class Event
	{
		private:
			uint32_t _id;
			uint32_t _curtain_length;
			uint32_t _desired_position;

		public:
			Event(uint32_t id, uint32_t curtain_length, uint32_t desired_position);

			uint32_t id();
			uint32_t desired_position();

			// ———— MOVEMENT ———— //
			bool event_moves_to_an_end();
			bool moves_full_span();
			CurtainState state_of_desired_position();
	};


	// declared here for Gpio.h functions, since Gpio.h is called in Curtain.h (and thus exists before it)
	class Curtain
	{
		private:
			// ———— OPTIONS ————
			bool _auto_calibrate;  // if the curtain has opportunity to move full span, count steps & return value
			bool _auto_correct;  // if position is unexpected, go to expected position
			// ———— CURRENT DATA ON CURTAIN ————
			uint32_t _current_position;  // the current length according to the RPi
			uint32_t _length;  // overall length of the curtain [steps]
			// ———— EVENT INFO ————
			Event _event;

		public:
			Curtain(StaticJsonDocument<Global::JSON_BUFFER_SIZE>&);
			char* serialize_data();

			// —————————————— GETTERS: ATTRIBUTES ——————————————
			bool calibrate();
			bool correct();

			uint32_t current_position();
			uint32_t length();
			Event event();

			// —————————————— GETTERS: DATA ——————————————
			bool should_calibrate_across();
			CurtainState state_of_current_position();

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
