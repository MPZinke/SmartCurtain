
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


#ifndef __EXCEPTIONS__
#define __EXCEPTIONS__


#include "Global.hpp"

#include "C_String.hpp"


namespace Exceptions
{
	void throw_generic(const char message[]);
	void throw_HTTP_204(const char message[]);
	void throw_HTTP_400(const char message[]);
	void throw_HTTP_404(const char message[]);
}  // end namespace Exceptions


#endif