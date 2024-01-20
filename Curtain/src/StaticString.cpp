

// #include "../Headers/StaticString.hpp"


#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdint.h>


#include "../Headers/CString.hpp"
#include "../Headers/DeserializedJSON.hpp"


template<size_t S>  // S is the total writable characters that may or may not be null terminated.
class StaticString
/*
- Adds safety by guaranteeing a null terminator at the index S.
	- CString::copy function will always null terminate at either the end of a c-string or the end of memory size.
- Can be used to fit string exactly (including null terminator) by templating <sizeof()-1>, which indicates a knowledge
  of how it works. IE specify allocation of <sizeof()-1>+1 for null terminating string.
- The premise of this class is to let you screw up your data, but not the program. The null terminator is partially
  hidden, and you can utilize it if you know what you are doing, but if you don't (EG <sizeof()-2>) then it will clip
  your string. However, to the lame man or just using it correctly and without trying to optimize, the only cost is an
  extra null terminator.
*/
{
	// Don't allow empty string, because it is a waste of StaticString.
	static_assert(0 < S && S+1 <= 0xFFFF, "Size S must be greater than 0 and `S + 1` less than or equal to 0xFFFF");

	public:
		StaticString()
		{
			char* string = (char*)_string;  // Sugar
			string[0] = '\0';
			string[S] = '\0';  // Always have an absolute null terminator
		}


		StaticString(const StaticString& static_string)
		{
			char* string = (char*)_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			_length = CString::copy(static_string._string, string, S+1);
		}


		StaticString(JsonObject& json_object)
		{
			char* string = (char*)_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			serializeJson(json_object, string, (size_t)JSON_BUFFER_SIZE);
			_length = CString::length(string, S+1);
		}


		StaticString(const char input1[], const char input2[], const char input3[])
		{
			char* string = (char*)_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			_length = CString::copy(input1, string, S+1);  // +1 because +1 bytes always allocated.
			_length += CString::copy(input2, string+_length, S+1-_length);  // +1 because +1 bytes always allocated.
			_length += CString::copy(input1, string+_length, S+1-_length);  // +1 because +1 bytes always allocated.
		}


		StaticString& operator+=(char right)
		{
			assert(_length < S);

			char* string = (char*)_string;  // Sugar

			string[_length] = right;
			_length++;
			string[_length] = '\0';  // Always null terminate

			return *this;
		}


		operator const char*()
		{
			return _string;
		}


		operator const char*() const
		{
			return _string;
		}


		operator DeserializedJSON::DeserializedJSON()
		{
			return DeserializedJSON::DeserializedJSON(*this);
		}


	private:
		uint16_t _length = 0;  // The number of human readable characters (a max of S).
		const char _string[S+1] = {};
};


template class StaticString<45>;
template class StaticString<46>;
template class StaticString<48>;
template class StaticString<50>;
template class StaticString<52>;
template class StaticString<JSON_BUFFER_SIZE>;
