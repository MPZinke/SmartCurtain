

#pragma once


#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdint.h>


namespace DeserializedJSON
{
	class DeserializedJSON;
}


// `S` is the maximum number of readable characters. `S+1` memory will be allocated so that the StaticString will be
//  guarenteed to be null terminated—even in the case where an empty StaticString is delcared and later written to.
template<size_t S>
class StaticString
{
	// Don't allow empty string, because it is a waste of StaticString.
	static_assert(0 < S && S+1 < 0xFFFF, "Size S must be greater than 0 and `S + 1` less than 0xFFFF");

	public:
		StaticString();
		StaticString(const StaticString& static_string);  // Copy constructor
		StaticString(JsonObject& json_object);
		StaticString(const char input1[], const char input2[], const char input3[]);

		StaticString& operator+=(char right);
		operator const char*();
		operator const char*() const;
		operator DeserializedJSON::DeserializedJSON();


	private:
		uint16_t _length = 0;  // The number of human readable characters (a max of S).
		const char _string[S+1];
};
