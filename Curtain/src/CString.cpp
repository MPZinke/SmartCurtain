

#include "../Headers/CString.hpp"


namespace CString
{
	uint16_t copy(const char source[], char destination[], uint16_t max/* =0xFFFF */)
	/*
	SUMMARY: Makes a safe copy of a c-string by always adding a null terminator.
	PARAMS:  Takes the string that will be copied, the location the string is copying to, the max length of copying to
	         occur.
	DETAILS: TODO
	RETURNS: The length of the data that was copied.
	*/
	{
		uint16_t x = 0;
		for(char current_character = source[0]; x < max && current_character != '\0'; x++)
		{
			current_character = source[x];
			destination[x] = current_character;
		}
		destination[x] = '\0';

		return x;  // Which at this point would equal max
	}


	uint16_t length(const char source[], uint16_t max/* =0xFFFF */)
	{
		for(uint16_t x = 0; x < max; x++)
		{
			if(source[x] == '\0')
			{
				return x;
			}
		}

		return max;  // Which at this point would equal max
	}
}
