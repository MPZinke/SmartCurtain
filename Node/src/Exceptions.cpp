
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
#include "Headers/Transmission.hpp"


namespace Excpetion
{
	void throw_generic(char message[])
	{
		// send
		delete[] message;
		longjmp(Global::jump_buffer, 1);
	}


	void throw_generic(const char message[])
	{

		longjmp(Global::jump_buffer, 1);
	}


	void throw_HTTP_204(const char message[])
	{

		Transmission::respond_with_json_and_stop((char*)Transmission::Literal::Responses::INVALID, Transmission
		  ::Literal::HTTP::NO_CONTENT_REQUEST);
		longjmp(Global::jump_buffer, 1);
	}


	void throw_HTTP_400(const char message[])
	{
		Transmission::respond_with_json_and_stop((char*)Transmission::Literal::Responses::INVALID, Transmission
		  ::Literal::HTTP::INVALID_REQUEST);
		longjmp(Global::jump_buffer, 1);
	}
}