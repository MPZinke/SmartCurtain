

#include "../Headers/CString.hpp"


namespace CString
{
	uint16_t copy(const char source[], char destination[], uint16_t max_characters/* =0xFFFF */)
	/*
	SUMMARY: Makes a safe copy of a c-string by always adding a null terminator unless the destination max length is 0.
	PARAMS:  Takes the string that will be copied, the location the string is copying to, the max length of copying to
	         occur.
	DETAILS: TODO
	RETURNS: The length of the data that was copied.
	*/
	{
		uint16_t x;
		char current_character;
		for(x = 0, current_character = source[0]; x < max_characters; x++, current_character = source[x])
		{
			destination[x] = current_character;
			if(current_character == '\0')
			{
				return x;
			}
		}

		return x;  // Which at this point would equal max_characters
	}


	uint16_t length(const char source[], uint16_t max_characters/* =0xFFFF */)
	{
		for(uint16_t x = 0; x < max_characters; x++)
		{
			if(source[x] == '\0')
			{
				return x;
			}
		}

		return max_characters;  // Which at this point would equal max_characters
	}
}
