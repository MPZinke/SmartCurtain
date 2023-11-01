

// #include "../Headers/StaticString.hpp"


#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdint.h>


#include "../Headers/DeserializedJSON.hpp"


template<size_t S>
class StaticString
{
	static_assert(S < 0xFFFF, "Size S of StaticString must be more than 0xFFFF");

	public:
		StaticString()
		: _length{0}
		{
			if(0 < S)
			{
				((char*)_string)[0] = '\0';
			}
		}


		StaticString(JsonObject& json_object)
		{
			serializeJson(json_object, (char*)_string, (size_t)JSON_BUFFER_SIZE);
		}


		StaticString(const char* input)
		{
			copy(input, (char*)_string);
		}


		StaticString(const char* input, const char* substring, uint16_t offset)
		{
			copy(input, (char*)_string, offset);
			copy(input, ((char*)_string)+offset);

			uint16_t substring_length = length(substring);
			copy(input+substring_length, ((char*)_string)+substring_length+offset);
		}


		void write(const char* source, uint16_t offset=0)
		{
			char current_char = source[0];
			for(uint16_t x = 0; x < S && current_char; x++, current_char = source[x])
			{
				((char*)_string)[offset+x] = current_char;
			}
		}


		StaticString& operator+=(char right)
		{
			assert(_length < S-1);
			((char*)_string)[_length] = right;
			_length++;

			return *this;
		}


		bool operator==(const char* right) const
		{
			char left_x = _string[0], right_x = right[0];
			for(uint16_t x = 0; x < 0xFFFF && left_x && right_x; x++)
			{
				if(left_x != right_x)
				{
					return false;
				}

				left_x = _string[x];
				right_x = right[x];
			}

			return left_x == right_x;
		}


		bool operator==(StaticString& right) const
		{
			return *this == right._string;
		}


		operator const char*()
		{
			return _string;
		}


		operator DeserializedJSON::DeserializedJSON()
		{
			return DeserializedJSON::DeserializedJSON(_string);
		}


		static void copy(const char* source, char* destination, uint16_t max=0xFFFF)
		{
			uint16_t x;
			char current_char;
			for(x = 0, current_char = source[0]; x < max && current_char; x++, current_char = source[x])
			{
				destination[x] = current_char;
			}

			destination[x] = '\0';
		}


		static uint16_t length(const char* source)
		{
			uint16_t x;
			for(x = 0; x < 0xFFFF; x++)
			{
				if(source[x] == '\0')
				{
					return x;
				}
			}

			return 0;
		}


		static void write(char* string, const char* substring, uint16_t offset=0)
		{
			char current_char = substring[0];
			for(uint16_t x = 0; x < 0xFFFF && current_char; x++, current_char = substring[x])
			{
				string[offset+x] = current_char;
			}
		}


	private:
		uint16_t _length = 0;
		const char _string[S] = {};
};


template class StaticString<0>;
template class StaticString<43>;
template class StaticString<45>;
template class StaticString<47>;
template class StaticString<49>;
template class StaticString<JSON_BUFFER_SIZE>;
