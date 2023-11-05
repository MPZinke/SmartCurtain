
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



#include <ArduinoJson.h>


#include "Config.hpp"


template<size_t S>
class StaticString;


namespace DeserializedJSON
{
	class DeserializedJSON
	{
		private:
			StaticJsonDocument<JSON_BUFFER_SIZE> _document;
			bool _ok;

		public:
			DeserializedJSON(StaticString<JSON_BUFFER_SIZE>& json);
			bool ok();

			JsonVariantConst operator[](int index) const;
			JsonVariantConst operator[](const char* key) const;
			bool containsKey(const char* key);
	};
}
