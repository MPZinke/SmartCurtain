
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


#include "../Headers/StaticString.hpp"


namespace DeserializedJSON
{
	DeserializedJSON::DeserializedJSON(StaticString<JSON_BUFFER_SIZE>& json)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		_ok = !deserializeJson(_document, (const char*)json);
	}


	bool DeserializedJSON::ok()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _ok;
	}


	JsonVariantConst DeserializedJSON::operator[](int index) const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _document[index];
	}


	JsonVariantConst DeserializedJSON::operator[](const char* key) const
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _document[key];
	}


	bool DeserializedJSON::containsKey(const char* key)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return _document.containsKey(key);
	}


	bool DeserializedJSON::is_valid_object_id(const char* key)
	{
		if(!_document[key].is<const char*>())
		{
			return false;
		}

		const char* value = _document[key];
		for(uint16_t x = 0; x < 24; x++)
		{
			char character = value[x];
			if(character < '0' || ('9' < character && character < 'a') || 'f' < character)
			{
				return false;
			}
		}

		return value[24] == '\0';
	}
}
