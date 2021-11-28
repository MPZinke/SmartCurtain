
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


namespace Exceptions
{
	void throw_generic(const char message[])
	{

		goto end_catch;
	}


	void throw_HTTP_204(const char message[])
	{

		Transmission::respond_with_json_and_stop(Transmission::Responses::INVALID_RESPONSE, );
		goto end_catch;
	}


	void throw_HTTP_400(const char message[])
	{

		goto end_catch;
	}
}  // end namespace Exceptions
