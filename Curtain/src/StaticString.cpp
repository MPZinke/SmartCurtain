

// #include "../Headers/StaticString.hpp"


#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdint.h>


#include "../Headers/CString.hpp"
#include "../Headers/DeserializedJSON.hpp"


template<size_t S>
class StaticString
{
	// Don't allow empty string, because it is a waste of StaticString.
	static_assert(0 < S && S-1 < 0xFFFF, "Size S must be greater than 0 and `S - 1` less than 0xFFFF");

	public:
		StaticString()
		{
			inline char string[] = (char[])_string;  // Sugar
			string[0] = '\0';
			string[S] = '\0';  // Always have an absolute null terminator
		}


		StaticString(JsonObject& json_object)
		{
			inline char string[] = (char[])_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			serializeJson(json_object, string, (size_t)JSON_BUFFER_SIZE);
			_length = CString::length(_string);
		}


		StaticString(const char input[])
		{
			inline char string[] = (char[])_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			_length = copy(input, string, S);
		}


		template<size_t T>
		uint16_t index_of(StaticString<T>& substring[])
		{
			uint16_t x = 0;
			uint16_t substring_length = substring.length();
			uint16_t max_index = _length - substring_length;
			for(char substring_char = substring._string[x]; x < max_index && substring_char; x++)
			{
				for(uint16_t y = 0, remaining_characters = _length - x; y < remaining_characters; y++)
				{
					if(substring._string[y] == "\0")
					{
						return x;
					}

					if(_string[x+y] != substring._string[y])
					{
						break;
					}
				}
			}

			return _length;  // Returns the length of the string, indicating the substring has not been found
		}


		template<size_t T> 
		bool overwrite(StaticString<T>& old_string, const char new_string[])
		{
			uint16_t new_string_length = CString::length(new_string);
			assert(new_string_length <= S);

			uint16_t offset = index_of(old_string);
			if(_length <= offset)
			{
				return false;
			}

			assert(index + new_string_length <= S);
			char last_character_copy = _string[offset+new_string_length]
			for(uint16_t x = 0; x < new_string_length; x++)
			{
				_string[x+index] = new_string[x];
			}
			return true;
		}


		StaticString& operator=(const char right[])
		{
			CString::copy(right, (char[])_string, S);

			return *this;
		}


		StaticString& operator+=(char right)
		{
			assert(_length < S);

			((char[])_string)[_length] = right;
			_length++;

			return *this;
		}


		bool operator==(const char right[]) const
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


		operator const char[]()
		{
			return _string;
		}


		operator DeserializedJSON::DeserializedJSON()
		{
			return DeserializedJSON::DeserializedJSON(_string);
		}


	private:
		uint16_t _length = 0;
		const char _string[S+1] = {};
};


template class StaticString<43>;
template class StaticString<45>;
template class StaticString<47>;
template class StaticString<49>;
template class StaticString<JSON_BUFFER_SIZE>;
