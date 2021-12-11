
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.11.27                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include "Global.hpp"


namespace Exceptions
{
	void throw_generic(const char message[]);
	void throw_HTTP_204(const char message[]);
	void throw_HTTP_400(const char message[]);
}  // end namespace Exceptions
