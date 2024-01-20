
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


#include "Config.hpp"


template<size_t S>
class StaticString;


namespace Exception
{
	extern void(*reset_function) (void);  // Move the program counter to 0x00


	class Exception
	{
		protected:
			uint32_t _line;
			const char* _file;
			const char* _message;

		public:
			Exception(uint32_t line, const char* file, const char* message);
			~Exception();

			operator StaticString<JSON_BUFFER_SIZE>();
	};
}  // end namespace Exceptions
