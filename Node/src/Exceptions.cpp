
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.10                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "Headers/Exceptions.hpp"


namespace Excpetion
{
	void throw_generic(const char message[])
	{

		longjump(Global::jump_buffer, 1);
	}


	void throw_HTTP_204(const char message[])
	{

		Transmission::respond_with_json_and_stop(Transmission::Responses::INVALID_RESPONSE, );
		longjump(Global::jump_buffer, 1);
	}


	void throw_HTTP_400(const char message[])
	{

		longjump(Global::jump_buffer, 1);
	}
}