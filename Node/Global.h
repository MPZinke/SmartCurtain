
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


// ————————————————————————————————————————————————————— C-STRING —————————————————————————————————————————————————————

namespace C_String
{
	// 
	void copy_n(char from[], char to[], uint8_t length)
	{
		for(uint8_t x = 0; x < length; x++) to[x] = from[x];
		to[length] = 0;
	}


	// LENGTH DOES NOT INCLUDE Null terminator.
	// The old tried and test string with the new twist of a better name. ;)
	// Takes a byte array (that is hopefully Null Terminated).
	// Iterates array until Null terminator is found or max length is reached.
	// Return length of string (or max uint8_t).
	uint8_t length(const char string[])
	{
		uint8_t length = 255;
		while(length && string[255-length]) length--;
		return 255-length;
	}


	// Finds the next occurance of a whitespace character.
	// Takes string pointer.
	// Iterates through string until whitespace found.
	// Returns position of whitespace char if found, else -1.
	int16_t next_white_space(char string[])
	{
		for(uint8_t x = 0; x <= 255; x++) if(string[x] == 32 || (9 < string[x] && string[x] <= 13)) return x;
		return -1;
	}


	// Get the first start position of a given string.
	// Takes a string to search, string to search for, lengths for setting for-loops.
	// Iterates through big string, matching characters until match found.
	// Returns position if found, otherwise -1 if match not found.
	int16_t position(const char haystack[], const char needle[], uint8_t haystack_length, uint8_t needle_length)
	{
		// search while haystack has enough length to fit
		for(uint8_t x = 0, y; x < haystack_length - needle_length; x++)
		{
			for(y = 0; y < needle_length; y++) if(haystack[x+y] != needle[y]) break;
			if(y == needle_length) return x;
		}
		return -1;  // you done goofed
	}


	// Get the first start position of a given string.
	// Takes a string to search, string to search for.
	// Determines length of passed strings. Iterates through big string, matching characters until match found.
	// Returns position if found, otherwise -1 if match not found.
	int16_t position(const char haystack[], const char needle[])
	{
		uint8_t haystack_length = C_String::length(string);  // use C_String::length for for_loop to prevent runaway
		uint8_t needle_length = C_String::length(needle_length);  // use C_String::length for for_loop to prevent runaway
		string_position(haystack, needle, haystack_length, needle_length);
	}
}


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

// ——————————————————————————————————————————————————————— JSON ———————————————————————————————————————————————————————

namespace Json
{
	bool is_json(char string[], uint8_t length)
	{
		if(string[0] != '{' || string[length-1] != '}') return false;
		//TODO: finish function
	}


	bool is_json(char string[])
	{
		return is_json(string, C_String::length(string));
	}


	// Finds the end of a string literal.
	int16_t literal_end(char string[])
	{
		bool escape = false;
		for(uint8_t x = 1; x < 255; x++)
		{
			if(string[x] == '\\') escape = !escape;
			else if(string[x] == '"' && !escape) return x+1;
		}
		return -1;
	}


	// Get the position of the value in a JSON.
	// Takes the json string to search in, the key string to search for.
	// Iterate c-string until position of key found.  Skip of key, [whitespace], colon, [whitespace].
	// Returns int16_t for -1 if key not found, otherwise index of value.
	int16_t position_of_value_for_key(char json[], const char key[])
	{
		int16_t key_position = C_String::position(json, key);
		if(key_position < 0) return -1;

		// skip to value position
		uint8_t index = key_position + C_String::length(key);
		while(index < 255 && json[index] == ' ') index++;  // skip possible whitespace
		if(json[index++] != ':') return -1;  // check that actual JSON && increment index
		while(index < 255 && json[index] == ' ') index++;

		return (index < 255) * index + (index == 255) * -1;  // return index if in of bounds, else -1 :)
	}


	// Get the value for a key in a json string.
	// Takes the json string to search, they key string to find.
	// 
	String value_for_key(char json[], const char key[])
	{
		// get & check start and end of string literal
		int16_t value_start = position_of_value_for_key(json, key);
		if(json[value_start] != '"') return String();  // ensure correct start
		int16_t value_end = literal_end(json+value_start);
		if(value_end < 0 || value_start == value_end) return String();

		char buffer[256];
		C_String::copy_n(json+value_start, buffer, value_end - value_start - 1);
		return String(buffer);
	}


	uint32_t value_for_key(char json[], const char key[])
	{
		int16_t value_start = position_of_value_for_key(json, key);
		if(value_start < 0) return 0;
		int16_t value_end = C_String::next_white_space(json+value_start);
		if(value_end < 0 || value_start == value_end) return 0;

		char buffer[256];
		C_String::copy_n(json+value_start, buffer, value_end - value_start);
		return atoi(buffer);
	}
}


// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

#define DEF(x) #x  // convert a define into a string

#endif
