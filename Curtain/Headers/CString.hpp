

#include <stdint.h>


namespace CString
{
	uint16_t copy(const char source[], char destination[], uint16_t max_characters=0xFFFF);
	uint16_t length(const char source[], uint16_t max_characters=0xFFFF);
}
