
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
#ifdef __ETHERNET__
	#include <Ethernet.h>
	#include <EthernetClient.h>
#elif __WIFI__
	#include <WiFi.h>
	#include <WiFiClient.h>
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
	uint8_t next_white_space(char[]);
	uint8_t position(char[], const char[], uint8_t, uint8_t);
	uint8_t position(char[], const char[]);


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

		// the old switch-a-roo
		for(uint8_t y = 0; y < (x+1) / 2; y++)
		{
			char left = to[y];
			to[y] = to[x-y];
			to[x-y] = left;
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


	// Finds the next occurance of a whitespace character.
	// Takes string pointer.
	// Iterates through string until whitespace found.
	// Returns position of whitespace char if found, else -1.
	uint8_t next_white_space(char string[])
	{
		for(uint8_t x = 0; x < 255; x++) if(string[x] == 32 || (9 <= string[x] && string[x] <= 13)) return x;
		return -1;
	}


	// Get the first start position of a given string.
	// Takes a string to search, string to search for, lengths for setting for-loops.
	// Iterates through big string, matching characters until match found.
	// Returns position if found, otherwise -1 if match not found.
	uint8_t position(char haystack[], const char needle[], uint8_t haystack_length, uint8_t needle_length)
	{
		assert(haystack_length < 255 && needle_length < 255);  // allow for '-1' to be returnable
		// search while haystack has enough length to fit
		for(uint8_t x = 0, y; x < haystack_length - needle_length; x++)
		{
			for(y = 0; y < needle_length; y++) if(haystack[x+y] != needle[y]) break;
			if(y == needle_length) return x;
		}
		return -1;  // (255) you done goofed
	}


	// Get the first start position of a given string.
	// Takes a string to search, string to search for.
	// Determines length of passed strings. Iterates through big string, matching characters until match found.
	// Returns position if found, otherwise -1 if match not found.
	uint8_t position(char haystack[], const char needle[])
	{
		// use C_String::length for for_loop to prevent runaway
		return position(haystack, needle, C_String::length(haystack), C_String::length((char*)needle));
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

#ifdef __ETHERNET__
	EthernetServer server(User::port);
#elif __WIFI__
	WiFiServer server(User::port);
#endif
	HttpClient* client = NULL;

} // end namespace Global


// ——————————————————————————————————————————————————————— JSON ———————————————————————————————————————————————————————

namespace Json
{
	// ————————————————————————————————————————————————— JSON: GLOBAL —————————————————————————————————————————————————

	// represents the type of the current and expected upcoming token in JSON format
	typedef enum
	{
		KEY,
		VALUE,
		COLON,
		COMMA
	} token_type;




	// ———— JSON: GETTERS ————
	uint8_t position_of_value_for_key(char[], const char[]);
	uint32_t value_for_key(char[], const char[]);
	// ———— JSON: TOKENIZING ————
	bool next_token_is_str_lit(char[], uint8_t, uint8_t&);
	bool next_token_is_int_lit(char[], uint8_t, uint8_t&);
	void skip_white_space(char[], uint8_t, uint8_t&);
	// ———— JSON: VALIDATION ————
	bool object_is_json(char[], uint8_t);
	bool object_is_json(char[]);


	// ————————————————————————————————————————————————— JSON: GETTERS —————————————————————————————————————————————————

	// Get the position of the value in a JSON.
	// Takes the json string to search in, the key string to search for.
	// Iterate c-string until position of key found.  Skip of key, [whitespace], colon, [whitespace].
	// Returns uint8_t for -1 if key not found, otherwise index of value.
	uint8_t position_of_value_for_key(char json[], const char key[])
	{
		uint8_t key_position = C_String::position(json, key);
		if(key_position < 0) return -1;

		// skip to value position
		uint8_t length = C_String::length((char*)key);
		uint8_t index = key_position + length;
		skip_white_space(json, length, index);
		if(json[index++] != ':') return -1;  // check that actual JSON && increment index
		skip_white_space(json, length, index);

		return (index < 255) * index + (index == 255) * -1;  // return index if in of bounds, else -1 :)
	}


	// Gets that value for a given key in the JSON.
	// Takes address of JSON string, address of key string.
	// Gets the positions of the value, given the key. Copies the characters in the value token.
	// Converts copied characters to uint32_t.
	uint32_t value_for_key(char json[], const char key[])
	{
		// determine value 
		uint8_t value_start = position_of_value_for_key(json, key);
		if(value_start == (uint8_t)-1) return 0;
		uint8_t value_end = C_String::next_white_space(json+value_start);
		if(value_end == (uint8_t)-1 || value_start == value_end) return 0;

		char buffer[256];
		C_String::copy_n(json+value_start, buffer, value_end - value_start);
		return atoi(buffer);
	}


	// ——————————————————————————————————————————————— JSON: TOKENIZING ———————————————————————————————————————————————

	// Determines whether next token is a string literal RegEx: "[^"\n]*".
	// Takes address of JSON string, length of JSON string, pointer to the index of the JSON string.
	// Determines whether the current token is a string literal.
	// Returns true if value is string literal & sets index, false otherwise.
	bool next_token_is_str_lit(char string[], uint8_t length, uint8_t& index)
	{
		if(string[index] != '"') return false;

		bool escape = false;
		for(uint8_t x = index+1; x < length; x++)
		{
			if(string[x] == '\n') return false;
			else if(string[x] == '\\') escape = !escape;
			else if(!escape && string[x] == '"')  // ending "
			{
				index = x + 1;
				return true;
			}
			else if(escape) escape = false;  // escape applies to something other than a '\' and '"'
		}
		return false;
	}


	// Determines whether next token is an unsigned int literal RegEx: [0-9]*.
	// Takes address of JSON string, length of JSON string, pointer to the index of the JSON string.
	// Determines whether the current token is an unsigned int literal.
	// Returns true if value is an unsigned int literal & sets index, false otherwise.
	bool next_token_is_int_lit(char string[], uint8_t length, uint8_t& index)
	{
		if(string[index] < 48 || 57 < string[index]) return false;  // check that it is start of INT_LIT

		for(uint8_t x = index + 1; x < length; x++)
		{
			// is white space or comma
			if(string[x] == ','|| string[x] == '}'  || string[x] == ' ' || (9 <= string[x] && string[x] <= 13))
			{
				index = x;
				return true;
			}
			else if(string[x] < 48 || 57 < string[x]) return false;  // non-int lit character
		}
		return false;
	}


	// Ignore white space at current index.
	// Takes address of JSON string, length of JSON string, pointer to the index of the JSON string.
	// Iterates until end of string or no more whitespace found.
	// Sets index.
	void skip_white_space(char string[], uint8_t length, uint8_t& index)
	{
		for(uint8_t x = index; x < length && (string[x] == 32 || (9 <= string[x] && string[x] <= 13)); x++) index = x+1;
	}


	// ——————————————————————————————————————————————— JSON: VALIDATION ———————————————————————————————————————————————

	// NOTE: It's more efficient not to, but this can be done with RegEx: {(STR_LIT : INT_LIT (, STR_LIT : INT_LIT)*)?}
	// Determines whether string is a correctly formatted object JSON.
	// Takes address of string, length of string.
	// Iterates over string, creating tokens & checking format using next expected token.  
	// Returns true if string is correctly formatted to, false otherwise.
	bool object_is_json(char string[], uint8_t length)
	{
		if(string[0] != '{') return false;  // easiest check of all

		// Go over string, parsing tokens into member (str_lit|int_lit), colon, comma.
		// Store index in string in 'index' variable & the next expected token with the sought_token_type variable.
		// x is used to limit max interations while not incrementing index each loop (so functions can set index past
		// end of tokens without any crazy decrementing before loop increment).
		for(uint8_t x = 0, index = 1, sought_token_type = KEY; x < 255 && index < length; x++)  // allow max 255 iters
		{
			skip_white_space(string, length, index);
			// check for proper ending of JSON Object  REGEX: \}[ \t\n\r\f]*
			if(string[index] == '}')
			{
				skip_white_space(string, length, ++index);  // skip rest of whitespace
				return sought_token_type == COMMA && index == length;  // end of JSON, whitespace ignored, should be end
			}
			else if(string[index] == ':')
			{
				if(sought_token_type != COLON) return false;
				sought_token_type = VALUE;
				index++;
			}
			else if(string[index] == ',')
			{
				if(sought_token_type != COMMA) return false;
				sought_token_type = KEY;
				index++;
			}
			else if(next_token_is_int_lit(string, length, index))
			{
				if(sought_token_type != VALUE) return false;
				sought_token_type = COMMA;
			}
			else if(next_token_is_str_lit(string, length, index))
			{
				if(sought_token_type != KEY) return false;
				sought_token_type = COLON;
			}
			else return false;  // no acceptable state found: reject state
		}
		return false;
	}


	// NOTE: It's more efficient not to, but this can be done with RegEx: {(STR_LIT : INT_LIT (, STR_LIT : INT_LIT)*)?}
	// Determines whether string is a correctly formatted object JSON.
	// Takes address of string.
	// Determines length of string. Iterates over string, creating tokens & checking format using next expected token.  
	// Returns true if string is correctly formatted to, false otherwise.
	bool object_is_json(char string[])
	{
		return object_is_json(string, C_String::length(string));
	}

} // end namespace Json


// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

#define DEF(x) #x  // convert a define into a string
