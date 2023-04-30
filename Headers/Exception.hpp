
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


namespace Exception
{
	class Exception
	{
		protected:
			uint32_t _line;
			String _file;
			String _message;

		public:
			Exception(uint32_t line, String file, String message);
			Exception(uint32_t line, String file, const char* message);
			virtual ~Exception();

			operator String();

			uint32_t line();
			String file();
			String message();

			virtual void send();

			// friend operator String(Exception* exception);
	};
}  // end namespace Exceptions
