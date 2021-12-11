
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Stores info that is included on other dependencies of the Node.ino file.  Breaks values into differnt *
*    namespaces to separate functionalities. Holds C-String custom functions (similar to classic C-String functions),  *
*    Curtain class declaration & other global stuff, Json functions for received message interpretation. JSON          *
*    functions are limited to JSON formats received for this project.                                                  *
*   NOTES:   - With uint8_t return values, 255 (-1) is often used as a "bad value". Therefore, iterations must stop at *
*              less than 255.  Otherwise, the 255th value and -1 will be ambiguous or the final x++ will overflow the  *
*              returned number or a bad number is returned.                                                            *
*   BUGS:                                                                                                              *
*   FUTURE:  - Consider expanding JSON functions to be less exclusive                                                  *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include <ArduinoJson.h>
#include <assert.h>
#include <esp_wifi.h>
#include <HttpClient.h>
#include <setjmp.h>
#include <SPI.h>
#include <stdint.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>


#include "Config.hpp"


// —————————————————————————————————————————————————— PREPROCESSOR —————————————————————————————————————————————————— //


// ———— SMARTINESS ————
#define CLOSE_ENDSTOP true
#define ENCODER true
#define OPEN_ENDSTOP false


// —————————————————————————————————————————————————————— GLOBAL ——————————————————————————————————————————————————————

namespace Global
{
	WiFiServer server(Config::Network::PORT);

	WiFiClient client;

	// Mutable curtain information
	uint32_t current_position = 0;  // holds current position
	uint32_t curtain_length = Config::Hardware::DEFAULT_LENGTH;

	const uint16_t JSON_BUFFER_SIZE = 0x1000;

	jmp_buf jump_buffer;
} // end namespace Global



// ————————————————————————————————————————————————————— C-STRING —————————————————————————————————————————————————————

namespace C_String
{
	// ——————————————————————————————————————————————— C-STRING: GLOBAL ———————————————————————————————————————————————

	void copy(const char[], char[]);
	void copy_n(const char[], char[], uint16_t);
	void itoa(uint32_t, char[]);
	uint16_t length(char[]);


	// ———————————————————————————————————————————— C-STRING: FUNCTIONS ———————————————————————————————————————————— //

	uint32_t atoi(char string[])
	{
		uint16_t length = C_String::length(string);
		register uint32_t integer = 0;
		for(uint8_t x = 0; x < length; x++)
		{
			integer *= 10;
			integer += string[x] - 48;
		}

		return integer;
	}


	// Copies one c string to another & null terminates.
	// Takes address of place to read from, address of place to write to.
	// Iterates over number of character reading then writing.  Null terminates "to" after 254 or Null found.
	void copy(const char from[], char to[])
	{
		register uint16_t x;
		for(x = 0; x < Global::JSON_BUFFER_SIZE && from[x]; x++) to[x] = from[x];
		to[x] = 0;
	}


	// Copies n number of character & null terminates.
	// Takes address of place to read from, address of place to write to, number of character to write.
	// Iterates over number of character reading then writing.  Null terminates "to" after n-chars written.
	void copy_n(const char from[], char to[], uint16_t length)
	{
		for(register uint16_t x = 0; x < length; x++) to[x] = from[x];
		to[length] = 0;
	}


	bool equal(const char a[], char b[])
	{
		register uint16_t x;
		for(x = 0; x < 0xFFFF && a[x]; x++)
		{
			if(a[x] != b[x]) return false;
		}

		return a[x] == b[x];
	}


	// Converts an int to a c-string.
	// Takes the integer to be converted, the address of the location to convert it to.
	// Converts from least signicicant bit first, then reverses chars.
	void itoa(uint32_t integer, char to[])
	{
		if(!integer)
		{
			*to = 48;
			to[1] = 0;
			return;
		}

		register uint16_t x;
		// OxFFFE because following for loop adds 1 (this prevents overflow)
		for(x = 0; x < 0xFFFE && integer; x++)
		{
			to[x] = (integer % 10) + 48;  // add character
			integer /= 10;
		}
		to[x--] = 0;  // null terminate and back step

		// the old switch-a-roo: switch character order since it was written backwards
		for(register uint16_t left_index = 0; left_index < (x+1) / 2; left_index++)
		{
			register uint16_t right_index = x - left_index;
			to[left_index] ^= to[right_index];
			to[right_index] ^= to[left_index];
			to[left_index] ^= to[right_index];
		}
	}


	// LENGTH DOES NOT INCLUDE Null terminator.
	// The old tried and test string with the new twist of a better name. ;)
	// Takes a byte array (that is hopefully Null Terminated).
	// Iterates array until Null terminator is found or max length is reached.
	// Return length of string (or max uint16_t).
	uint16_t length(char string[])
	{
		uint16_t length = Global::JSON_BUFFER_SIZE;
		while(length && string[Global::JSON_BUFFER_SIZE-length]) length--;
		return Global::JSON_BUFFER_SIZE-length;
	}

} // end namespace C_String

