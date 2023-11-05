
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
}
