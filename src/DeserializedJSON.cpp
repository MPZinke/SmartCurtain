
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



#include "../Headers/DeserializedJSON.hpp"


namespace DeserializedJSON
{
	DeserializedJSON::DeserializedJSON(String json)
	{
		_ok = !deserializeJson(_document, json);
	}


	bool DeserializedJSON::ok()
	{
		return _ok;
	}


	StaticJsonDocument<JSON_BUFFER_SIZE> DeserializedJSON::document()
	{
		return _document;
	}


	JsonVariantConst DeserializedJSON::operator[](int index) const
	{
		return _document[index];
	}


	JsonVariantConst DeserializedJSON::operator[](const char* key) const
	{
		return _document[key];
	}


	bool DeserializedJSON::containsKey(const char* key)
	{
		return _document.containsKey(key);
	}
}
