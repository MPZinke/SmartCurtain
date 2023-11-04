

#include "../Headers/CString.hpp"


namespace CString
{
	uint16_t copy(const char source[], char destination[], uint16_t max=0xFFFF)
	/*
	SUMMARY: Makes a safe copy of a c-string by always adding a null terminator unless the destination max length is 0.
	PARAMS:  Takes the string that will be copied, the location the string is copying to, the max length of copying to
	         occur.
	DETAILS: TODO
	RETURNS: The length of the data that was copied.
	*/
	{
		// `1` is always substracted from `max` so that there is always room left for a null terminator, except in the
		//  case of `max == 0`. This is because if the max is 0, there is no room for a null terminator.
		if(max != 0)
		{
			max--;
		}

		uint16_t x = 0;
		for(char current_char = source[0]; x < max && current_char; x++, current_char = source[x])
		{
			destination[x] = current_char;
		}
		destination[x] = '\0';

		return x;
	}


	uint16_t length(const char source[])
	{
		for(uint16_t x = 0; x < 0xFFFF; x++)
		{
			if(source[x] == '\0')
			{
				return x;
			}
		}

		return 0;
	}
}
