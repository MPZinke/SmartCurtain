

#include <Arduino.h>


#include "../Headers/CString.hpp"


namespace CString
{
	uint16_t copy(const char source[], char destination[], uint16_t size)
	/*
	SUMMARY: Makes a safe copy of a c-string by always adding a null terminator at the end of the string or at the size
	         character.
	PARAMS:  Takes the string that will be copied, the location the string is copying to, the size length of memory to
	         copy to.
	DETAILS: TODO
	RETURNS: The length of the human readable data that was copied (ignoring the null terminator).
	*/
	{
		// No room for even a null terminator.
		if(size == 0)
		{
			 return 0;
		}

		uint16_t x = 0;
		for(char current_character = *source; x < size-1 && current_character != '\0'; x++)
		{
			current_character = source[x];
			destination[x] = current_character;
		}
		destination[x] = '\0';  // Here to guarantee null terminating if any memory available.

		return x-1;  // Copied x characters, but not stating the null terminator.
	}


	uint16_t length(const char source[], uint16_t size)
	{
		for(uint16_t x = 0; x < size; x++)
		{
			if(source[x] == '\0')
			{
				return x;
			}
		}

		assert(false);  // I made a mistake if I got to this point...
	}
}
