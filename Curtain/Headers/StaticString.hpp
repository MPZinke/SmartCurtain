

#pragma once


#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdint.h>


namespace DeserializedJSON
{
	class DeserializedJSON;
}


template<size_t S>
class StaticString
{
	// Don't allow empty string, because it is a waste of StaticString.
	static_assert(0 < S && S-1 < 0xFFFF, "Size S must be greater than 0 and `S - 1` less than 0xFFFF");

	public:
		StaticString();
		StaticString(JsonObject& json_object);
		StaticString(const char* input);

		void write(const char* input, uint16_t offset=0);

		StaticString& operator+=(char right);
		bool operator==(const char* right) const;
		bool operator==(String& right) const;
		bool operator==(StaticString& right) const;
		operator const char*();
		operator DeserializedJSON::DeserializedJSON();

	private:
		uint16_t _length = 0;
		const char _string[S+1];
};
