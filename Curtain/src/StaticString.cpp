

// #include "../Headers/StaticString.hpp"


#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdint.h>


#include "../Headers/CString.hpp"
#include "../Headers/DeserializedJSON.hpp"


template<size_t S>  // S is the total writable characters that may or may not be null terminated.
class StaticString
{
	// Don't allow empty string, because it is a waste of StaticString.
	static_assert(0 < S && S+1 < 0xFFFF, "Size S must be greater than 0 and `S + 1` less than 0xFFFF");

	public:
		void overwrite(const char old_string[], const char new_string[])
		{
			// Using `_length+1` to trivially test if the string is too long and not wasting possible excess cycles.
			uint16_t old_string_length = CString::length(old_string, _length+1);
			uint16_t new_string_length = CString::length(new_string, _length+1);
			assert(old_string_length == new_string_length);

			uint16_t old_string_index = index_of(old_string);
			assert(old_string_index != _length);  // Ensure the old_string is found

			CString::copy(new_string, ((char*)_string)+old_string_index, new_string_length);
		}


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

			_length = CString::copy(static_string._string, string, S);
		}


		StaticString(JsonObject& json_object)
		{
			char* string = (char*)_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			serializeJson(json_object, string, (size_t)JSON_BUFFER_SIZE);
			_length = CString::length(_string);
		}


		StaticString(const char input[])
		{
			char* string = (char*)_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			_length = CString::copy(input, string, S);
		}


		StaticString(const char input[], const char substring[], const char replacement[])
		{
			char* string = (char*)_string;  // Sugar
			string[S] = '\0';  // Always have an absolute null terminator

			_length = CString::copy(input, string, S);

			overwrite(substring, replacement);
		}


		uint16_t index_of(const char substring[])
		{
			uint16_t substring_length = CString::length(substring, _length+1);
			// If the substring is longer than the StaticString's length, return out of bounds
			if(substring_length >= _length)
			{
				return _length;
			}

			uint16_t x = 0;
			for(uint16_t max_search_index = _length - substring_length; x < max_search_index && _string[x]; x++)
			{
				for(uint16_t y = 0, remaining_characters = _length - x; y < remaining_characters; y++)
				{
					char substring_char = substring[y];
					if(substring_char == '\0')
					{
						return x;
					}

					if(_string[x+y] != substring_char)
					{
						break;
					}
				}
			}

			return _length;  // Returns the length of the string, indicating the substring has not been found
		}


		template<size_t T>
		void overwrite(StaticString<T>& old_string, const char new_string[])
		{
			overwrite(old_string._string, new_string);
		}


		StaticString& operator=(const char right[])
		{
			CString::copy(right, (char*)_string, S);

			return *this;
		}


		StaticString& operator+=(char right)
		{
			assert(_length < S);

			((char*)_string)[_length] = right;
			_length++;

			return *this;
		}


		bool operator==(const char right[]) const
		{
			char left_x = _string[0], right_x = right[0];
			for(uint16_t x = 0; x < 0xFFFF && left_x == right_x; x++, left_x = _string[x], right_x = right[x])
			{
				if(left_x == '\0' /* && right_x == '\0' */)  // Implicit
				{
					return true;
				}
			}

			return false;
		}


		bool operator!=(const char right[]) const
		{
			return !(*this == right);
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
		uint16_t _length = 0;
		const char _string[S+1] = {};
};


template class StaticString<25>;
template class StaticString<43>;
template class StaticString<45>;
template class StaticString<47>;
template class StaticString<49>;
template class StaticString<JSON_BUFFER_SIZE>;
