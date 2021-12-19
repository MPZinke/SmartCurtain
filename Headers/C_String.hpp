
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


#ifndef __C_STRING__
#define __C_STRING__


#include <stdint.h>

#include "Global.hpp"


namespace C_String
{
	uint32_t atoi(char string[]);
	void copy(const char from[], char to[]);
	void copy_n(const char from[], char to[], uint16_t length);
	bool equal(const char a[], char b[]);
	void itoa(uint32_t integer, char to[]);
	uint16_t length(char string[]);
};

#endif
