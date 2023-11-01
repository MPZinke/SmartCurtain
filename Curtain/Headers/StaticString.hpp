

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
	static_assert(S < 0xFFFF, "Size S of StaticString must be less than 0xFFFF");

	public:
		StaticString();
		StaticString(JsonObject& json_object);
		StaticString(const char* input);
		StaticString(const char* input, const char* substring, uint16_t offset);

		void write(const char* input, uint16_t offset=0);

		StaticString& operator+=(char right);
		bool operator==(const char* right) const;
		bool operator==(String& right) const;
		bool operator==(StaticString& right) const;
		operator const char*();
		operator DeserializedJSON::DeserializedJSON();

		static void copy(const char* source, char* destination, uint16_t max=0xFFFF);
		static uint16_t length(const char* input);
		static void write(char* string, const char* substring, uint16_t offset=0);

	private:
		uint16_t _length = 0;
		const char _string[S];
};
