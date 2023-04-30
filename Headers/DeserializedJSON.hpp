
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


namespace DeserializedJSON
{
	class DeserializedJSON
	{
		private:
			StaticJsonDocument<JSON_BUFFER_SIZE> _document;
			bool _ok;

		public:
			DeserializedJSON(String json);
			bool ok();
			StaticJsonDocument<JSON_BUFFER_SIZE> document();
			JsonVariantConst operator[](int index) const;
			JsonVariantConst operator[](const char* key) const;
			bool containsKey(const char* key);
	};
}
