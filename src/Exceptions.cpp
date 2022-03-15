
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


#include "../Headers/C_String.hpp"
#include "../Headers/Exceptions.hpp"
#include "../Headers/Request.hpp"


namespace Exceptions
{
	void throw_generic(String message)
	{
		using namespace Request;
		respond_with_json_and_stop(message, Literal::HTTP::NO_CONTENT_REQUEST);
		longjmp(Global::jump_buffer, 1);
	}


	void throw_generic(const char message[])
	{

		longjmp(Global::jump_buffer, 1);
	}


	void throw_HTTP_204(const char message[])
	{
		using namespace Request;
		respond_with_json_and_stop(Literal::Responses::INVALID, Literal::HTTP::NO_CONTENT_REQUEST);
		longjmp(Global::jump_buffer, 1);
	}


	void throw_HTTP_400(const char message[])
	{
		using namespace Request;
		respond_with_json_and_stop(Literal::Responses::INVALID, Literal::HTTP::INVALID_REQUEST);
		longjmp(Global::jump_buffer, 1);
	}


	void throw_HTTP_404(const char message[])
	{
		using namespace Request;
		respond_with_json_and_stop(Literal::Responses::INVALID, Literal::HTTP::NOT_FOUND_REQUEST);
		longjmp(Global::jump_buffer, 1);
	}
}