
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


#include <stdint.h>


#include "Global.hpp"


namespace C_String
{
	uint32_t atoi(const char string[]);
	void copy(const char from[], char to[]);
	bool equal(const char a[], const char b[]);
	void IP_address_octets(const char* source, uint8_t octets[4])
	void itoa(uint32_t integer, char to[]);
	uint16_t length(char string[]);
};
