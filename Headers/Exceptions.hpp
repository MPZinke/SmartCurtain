
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

			void send();
	};


	class HTTP_Exception: public Exception
	{
		protected:
			uint16_t _status_code;

		public:
			HTTP_Exception(uint32_t line, String file, String message, uint16_t status_code);
			~HTTP_Exception();

			String message();
			String send_message();
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
