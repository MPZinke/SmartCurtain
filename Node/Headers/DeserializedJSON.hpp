
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


#pragma once


#include <ArduinoJson.h>


#include "Config.hpp"


class DeserializedJSON
{
	private:
		StaticJsonDocument<JSON_BUFFER_SIZE> _document;
		bool _ok;

	public:
		DeserializedJSON(String json)
		{
			_ok = !deserializeJson(_document, json);
		}

		bool ok()
		{
			return _ok;
		}

		StaticJsonDocument<JSON_BUFFER_SIZE> document()
		{
			return _document;
		}


		FORCE_INLINE JsonVariantConst operator[](int index) const
		{
			return _document[index];
		}


		FORCE_INLINE JsonVariantConst operator[](const char* key) const
		{
			return _document[key];
		}


		bool containsKey(const char* key)
		{
			return _document.containsKey(key);
		}
};
