
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


#include <stdint.h>
#include <Arduino.h>


namespace Exceptions
{
	class Exception
	{
		protected:
			uint32_t _line;
			String _file;
			String _message;

		public:
			Exception(uint32_t line, String file, String message);

			uint32_t line();
			String file();
			String message();

			virtual void send();
	};


	class HTTP_Exception: public Exception
	{
		protected:
			const char* _request_header = NULL;
			uint16_t _status_code;

		public:
			HTTP_Exception(uint32_t line, String file, String message, uint16_t status_code,
			  const char* request_header);
			~HTTP_Exception();

			String message();
			void send();
	};


	class BAD_REQUEST_400_Exception: public HTTP_Exception
	{
		private:
			uint16_t _status_code = 400;

		public:
			BAD_REQUEST_400_Exception(uint32_t line, String file, String message);
	};


	class NOT_FOUND_404_Exception: public HTTP_Exception
	{
		private:
			uint16_t _status_code = 404;

		public:
			NOT_FOUND_404_Exception(uint32_t line, String file, String message);
	};


	class INTERNAL_SERVER_ERROR_500_Exception: public HTTP_Exception
	{
		private:
			uint16_t _status_code = 500;

		public:
			INTERNAL_SERVER_ERROR_500_Exception(uint32_t line, String file, String message);
	};
}  // end namespace Exceptions
