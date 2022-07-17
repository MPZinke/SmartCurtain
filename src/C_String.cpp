
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.19                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/C_String.hpp"

#include "../Headers/Config.hpp"


namespace C_String
{
	// ———————————————————————————————————————————— C-STRING: FUNCTIONS ———————————————————————————————————————————— //

	uint32_t atoi(const char string[])
	{
		uint16_t length = C_String::length((char*)string);
		register uint32_t integer = 0;
		for(uint8_t x = 0; x < length; x++)
		{
			integer *= 10;
			integer += string[x] - '0';
		}

		return integer;
	}


	// Copies one c string to another & null terminates.
	// Takes address of place to read from, address of place to write to.
	// Iterates over number of character reading then writing.  Null terminates "to" after 254 or Null found.
	void copy(const char from[], char to[])
	{
		register uint16_t x;
		for(x = 0; x < JSON_BUFFER_SIZE && from[x]; x++)
		{
			to[x] = from[x];
		}
		to[x] = 0;
	}


	bool equal(const char a[], const char b[])
	{
		register uint16_t x;
		for(x = 0; x < 0xFFFF && a[x] && a[x] == b[x]; x++)
		{
			if(a[x] != b[x])
			{
				break;
			}
		}

		return a[x] == b[x];
	}


	void IP_address_octets(const char* source, uint8_t octets[4])
	{
		uint16_t source_length = length((char*)source);
		for(uint8_t octet_index = 0, source_index = 0; octet_index < 4 && source_index < source_length;
		  octet_index++, source_index++)
		{
			char octet_string[4] = {'\0', '\0', '\0', '\0'};
			for(uint8_t string_index = 0; string_index < 3 && '0' <= source[source_index]
			  && source[source_index] <= '9'; string_index++, source_index++)
			{
				octet_string[string_index] = source[source_index];
			}
			octets[octet_index] = (uint8_t)atoi(octet_string);
		}
	}


	// Converts an int to a c-string.
	// Takes the integer to be converted, the address of the location to convert it to.
	// Converts from least signicicant bit first, then reverses chars.
	void itoa(uint32_t integer, char to[])
	{
		if(!integer)
		{
			*to = '0';
			to[1] = 0;
			return;
		}

		register uint16_t x;
		// OxFFFE because following for loop adds 1 (this prevents overflow)
		for(x = 0; x < 0xFFFE && integer; x++)
		{
			to[x] = (integer % 10) + '0';  // add character
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
		uint16_t length = JSON_BUFFER_SIZE;
		for(length = JSON_BUFFER_SIZE; length && string[JSON_BUFFER_SIZE-length]; length--)
		{}

		return JSON_BUFFER_SIZE-length;
	}
}  // end namespace C_String
