
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.25                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Read and write functions for communication between Raspberry Pi and Arduino. Second iteration for     *
*    comms functions for current curtain version.                                                                      *
*    This is used for responses in the form of a JSON object returned from a POST request which are then parsed.       *
*    Current revisions uses format is JSON Object (yes I know that's redundant, but that's what its called).           *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/

#ifndef _Transmission_
#define _Transmission_

#include "Global.h"


namespace Transmission
{
	// ———— CONNECTION ————
	void ensure_connection();
	// ———— SENDING/RECEIVING ————
	void post_data(char[]);
	void post_data(String);
	bool read_state_response_successfully_into_buffer(byte[]);
	// ———— UTILITY ————
	bool buffer_matches_string(const char[], uint8_t);
	bool buffer_matches_string(const char[]);
	byte buffer_mismatches_string(const char[], uint8_t);
	byte buffer_mismatches_string(const char[]);
	bool checksum_packet(byte[]);
	bool clear_and_return_false();
	bool clear_buffer_and_return_true();
	uint8_t message_length();
	uint64_t message_length(uint8_t);
	bool first_line_is_invalid();
	bool return_whether_buffer_is_empty_and_clear_it_if_not();

	// ————————————————————————————————————————————— TRANSMISSION: GLOBAL —————————————————————————————————————————————

	// ———— STRING LITERALS ————
	const char CONTENT_LENGTH_CONST_CSTR[] = "Content-Length: ";
	const char VALID_RESPONSE_STR[] = "HTTP/1.1 200 OK";  // initial string for valid response from device

	// ———— ENCODING ————
	// Designed for:
	//	{"curtain" : 99, "length" : 4294967295, "current position" : 4294967295, "desired position" : 4294967295, 
	//   "direction" : 1, "auto calibrate" : 1, "auto correct" : 1}
	// — OR —
	//	{"curtain":1,"length":0,"current position":0,"desired position":0,"direction":1,"auto calibrate":1,
	//   "auto correct":1}

	// if MIN_PACKET_LENGTH ever changes, change Transmissions::message_length(.) to match number of digits
	const uint8_t MIN_PACKET_LENGTH = 116;  // every valid packet received will have at least this amount of chars
	const uint8_t BUFFER_LENGTH = 255;  // should be a max of 96(97) chars

	const char CURRENT_POS_KEY[] = "\"current position\"";
	const char DESIRED_POS_KEY[] = "\"desired position\"";
	const char LENGTH_KEY[] = "\"length\"";

	const char CALIBRATE_KEY[] = "\"auto calibrate\"";
	const char CORRECT_KEY[] = "\"auto correct\"";
	const char DIRECTION_KEY[] = "\"direction\"";


	enum Options
	{
		NO_TOUCHING = 1,  // place holder as to not send null terminator (see description above)
		CALIBRATE = 2,  // bit of bool values for options in transmission. bit-and-ed with options char
		CORRECT = 4,  // bit of bool values for options in transmission. bit-and-ed with options char
		DIRECTION = 8  // bit of bool values for options in transmission. bit-and-ed with options char
	};


	// —————————————————————————————————————————————————— CONNECTION ——————————————————————————————————————————————————

	// Checks whether server is connected.
	// If not, continually tries to connect to server.
	void ensure_connection()
	{
		while(!Global::client.connected()) 
		{
			delay(1000);
			Global::client.connect(Global::server, 80);
		}
	}


	// ——————————————————————————————————————————————— SENDING/RECEIVING ———————————————————————————————————————————————

	// Sends data using POST method to HOST.
	// Takes char array of data to post.  Prints data to client.
	// Prints to client as per https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/POST
	void post_data(char data[])
	{
		ensure_connection();

		Global::client.print("POST ");
		Global::client.print(User::page);
		Global::client.println(" HTTP/1.1");

		Global::client.print("Host: ");
		Global::client.println(User::master_host_cstr);

		Global::client.println("Content-Type: application/x-www-form-urlencoded");
		Global::client.print("Content-Length: ");
		Global::client.println(C_String::length(data));
		Global::client.println();
		Global::client.print(data);
		Global::client.println();
	}


	// Sends data using POST method to HOST.
	// Takes char array of data to post.  Prints data to client.
	void post_data(String data)
	{
		post_data(data.c_str());
	}


	// Reads the incoming transmission from the response.
	// Takes pointer to the buffer where the scraped data will be stored.
	// Checks that the data transmission is of the correct format.
	// Returns true if valid. 
	bool read_state_response_successfully_into_buffer(byte packet_buffer[])
	{
		while(!Global::client.available());  // wait for reponse
		if(first_line_is_invalid()) return clear_and_return_false();
		uint8_t length = message_length();
		if(length < MIN_PACKET_LENGTH) return clear_and_return_false();  // ignore header & get content length

		// FROM: https://en.wikipedia.org/wiki/HTTP_message_body
		//  The content length's last char & message body's first char will have 2 new lines between them.
		//  message_length() only consumes a new-line if the number of digits of PACKET_LENGTH do no match buffer.
		//  Therefore, for a valid packet, there should be 2 new lines then the message.

		// while not two consecutive new-lines, ignore left-over headers if able to
		// message_length() should end before eats up \n. If it doesn't, something is wrong & rest is ignored
		while(Global::client.available() >= 2 && (Global::client.read() != '\n' || Global::client.read() != '\n'));
		if(Global::client.available() <= 2) return clear_and_return_false();  // read until the end (shouldn't happen)

		for(int x = 0; Global::client.available() && x < BUFFER_LENGTH; x++) packet_buffer[x] = Global::client.read();

		if(Global::client.available()) return false;  // should always be empty, but let's be prudent :D
		return Json::is_object_json((const char*) packet_buffer, length);
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	// THIS DOES NOT SAVE THE READ DATA
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with, the length of said string.
	// Iterates through C-string, checking that each byte matches.
	// Returns true if string matches buffer, false otherwise.
	bool buffer_matches_string(const char compare_string[], uint8_t length)
	{
		if(Global::client.available() < length) return false;  // not != b/c might want to pull more after match
		for(uint8_t x = 0; x < length; x++) if(compare_string[x] != Global::client.read()) return false;

		return true;
	}


	// THIS DOES NOT SAVE THE READ DATA
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with.
	// Determines the length of the string. Iterates through C-string, checking that each byte matches.
	// Returns true if string matches buffer, false otherwise.
	bool buffer_matches_string(const char compare_string[])
	{
		return buffer_matches_string(compare_string, C_String::length(compare_string));
	}


	// THIS ONLY RETURNS THE LAST CHARACTER READ THAT DOES NOT MATCH STRING
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with, the length of said string.
	// Iterates through C-string, checking that each byte matches.
	// Returns NULL (no mismatch) if the string matches the buffer, ELSE the last seen character, if mismatch or 1 if 
	// read is unavailable.
	byte buffer_mismatches_string(const char compare_string[], uint8_t length)
	{
		uint8_t x;
		byte previous;
		for(x = 0; Global::client.available() && x < length; x++)
		{
			previous = Global::client.read();
			if(compare_string[x] != previous) return previous;
		}
		return x != length;  // return NULL if x is the length of string, 1 if x is not length of string
	}


	// THIS ONLY RETURNS THE LAST CHARACTER READ THAT DOES NOT MATCH STRING
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with.
	// Determines the length of the string. Iterates through C-string, checking that each byte matches.
	// Returns NULL (no mismatch) if the string matches the buffer, ELSE the last seen character, if mismatch or 1 if
	// read is unavailable.
	byte buffer_mismatches_string(const char compare_string[])
	{
		return buffer_mismatches_string(compare_string, C_String::length(compare_string));
	}


	// SUGAR: Clears the buffer of remaining character & returns false.
	// Iterates through buffer until empty.
	// Returns false (so that calling function can return it (and false)).
	bool clear_and_return_false()
	{
		while(Global::client.available()) Global::client.read();
		return false;
	}


	// SUGAR: Clears the buffer of remaining character & returns true.
	// Iterates through buffer until empty.
	// Returns true (so that calling function can return it (and true)).
	bool clear_buffer_and_return_true()
	{
		while(Global::client.available()) Global::client.read();
		return true;
	}


	// THIS DOES NOT SAVE THE READ DATA & COULD CONSUME AN EXTRA NEWLINE IF CONTENT LENGTH < 100
	// Gets the content length of the message.
	// Reads header (or all) in until Content-Lenght string is found.  Then gets the next three bytes/numbers unless a 
	// new-line is encountered.  Then converts the read in bytes to a uint8_t.
	// Returns length of the message if parsed properly, otherwise 0.
	uint8_t message_length()
	{
		// skip while at wrong part
		while(Global::client.available() && buffer_mismatches_string(CONTENT_LENGTH_CONST_CSTR));

		char number_buffer[4] = {0, 0, 0, 0};  // zero out for arbitrary number between [0, 999]
		// (get next three bytes OR stop at newline) if buffer available (should be, but double check :) )
		for(uint8_t x = 0; x < 3 && (x == 0 || number_buffer[x-1] != '\n') && Global::client.available(); x++)
		{
			number_buffer[x] = Global::client.read();
		}
		return atoi(number_buffer);
	}


	// THIS DOES NOT SAVE THE READ DATA & COULD CONSUME AN EXTRA NEWLINE IF log(CONTENT LENGTH) < max_digits
	// Gets the content length of the message.
	// Reads header (or all) in until Content-Lenght string is found.  Then gets the next three bytes/numbers unless a 
	// new-line is encountered.  Then converts the read in bytes to a uint8_t.
	// Returns length of the message if parsed properly, otherwise 0.
	uint64_t message_length(uint8_t max_digits)
	{
		assert(max_digits <= 20);  // number of base-10 digits for a uint64_t
		// skip while at wrong part
		while(Global::client.available() && buffer_mismatches_string(CONTENT_LENGTH_CONST_CSTR));

		// zero out for arbitrary number between [0, 10^max_digits]
		char* number_buffer = (char*)calloc(max_digits+1, sizeof(char));
		// (get next three bytes OR stop at newline) if buffer available (should be, but double check :) )
		for(uint8_t x = 0; x < max_digits && (x == 0 || number_buffer[x-1] != '\n') && Global::client.available(); x++)
		{
			number_buffer[x] = Global::client.read();
		}

		uint64_t length = atoi(number_buffer);
		free(number_buffer);  // almost forgot to do this

		return length;
	}


	// Compare first line of response with known response to see if successful.
	// Uses defined string to compare each character of response for accuracty.
	// Returns if they do not match.
	bool first_line_is_invalid()
	{
		return buffer_matches_string(VALID_RESPONSE_STR, C_String::length(VALID_RESPONSE_STR));
	}


	// SUGAR: Name should explain what it does.
	// Returns true if buffer is empty, else false if not empty.
	bool return_whether_buffer_is_empty_and_clear_it_if_not()
	{
		if(!Global::client.available()) return true;
		return clear_and_return_false();
	}

}  // end namespace Transmission

#endif
