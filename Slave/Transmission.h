
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.25                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Read and write functions for communication between Raspberry Pi and Arduino. Second iteration for     *
*    comms functions for current curtain version.  Transmissions are sent adding 1 to each number to prevent a null    *
*    terminator from being sent—I've had problems with this in the past.                                               *
*    This is used for responses in the form of base128 numbers sent as chars which are then parsed.                    *
*    Current revisions uses format <options: 1> <Current Position: 3> <Length: 3> <Desired Position: 3> <Checksum: 1>. *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/

#ifndef _Transmission_
#define _Transmission_

#include "assert.h"
#include "Global.h"

namespace Transmission
{
	void ensure_connection();
	void post_data(String);
	void post_data(char[]);
	bool read_state_response_successfully_into_buffer(byte[]);
	bool buffer_matches_string(const char[], uint8_t);
	bool buffer_matches_string(const char[]);
	byte buffer_mismatches_string(const char[], uint8_t);
	byte buffer_mismatches_string(const char[]);
	bool checksum_packet(byte[]);
	bool clear_buffer_and_return_false();
	bool clear_buffer_and_return_true();
	uint8_t message_length();
	uint64_t message_length(uint8_t);
	bool first_line_is_invalid();
	bool return_whether_buffer_is_empty_and_clear_it_if_not();
	uint8_t string_length(const char[]);

	// ————————————————————————————————————————————— TRANSMISSION: GLOBAL —————————————————————————————————————————————

	// ———— STRING LITERALS ————
	const char CONTENT_LENGTH_CONST_CSTR[] = "Content-Length: ";
	const char VALID_RESPONSE_STR[] = "HTTP/1.1 200 OK";  // initial string for valid response from device

	// ———— ENCODING ————
	// if PACKET_LENGTH ever changes, change Transmissions::message_length(.) to match number of digits
	const uint8_t PACKET_LENGTH = 11;

	const uint8_t OPTIONS = 0;  // location in transmission of options bits
	const uint8_t CURRENT_LOW = 1;  // location in transmission of lower 7 bits of current known curtain position
	const uint8_t CURRENT_MID = 2;  // location in transmission of middle 7 bits of current known curtain position
	const uint8_t CURRENT_UP = 3;  // location in transmission of upper 7 bits of current known curtain position
	const uint8_t LENGTH_LOW = 4;  // location in transmission of lower 7 bits of curtain length
	const uint8_t LENGTH_MID = 5;  // location in transmission of middle 7 bits of curtain length
	const uint8_t LENGTH_UP = 6;  // location in transmission of upper 7 bits of curtain length
	const uint8_t DESIRED_LOW = 7;  // location in transmission of lower 7 bits of desired curtain position
	const uint8_t DESIRED_MID = 8;  // location in transmission of middle 7 bits of desired curtain position
	const uint8_t DESIRED_UP = 9;  // location in transmission of upper 7 bits of desired curtain position
	const uint8_t CHECKSUM = 10;  // always has to be a the end (see checksum_packet())


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
	void post_data(String data)
	{
		post_data(data.c_str());
	}


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
		Global::client.println(string_length(data));
		Global::client.println();
		Global::client.print(data);
		Global::client.println();
	}


	// Reads the incoming transmission from the response.
	// Takes pointer to the buffer where the scraped data will be stored.
	// Checks that the data transmission is of the correct format.
	// Returns true if valid. 
	bool read_state_response_successfully_into_buffer(byte packet_buffer[])
	{
		while(!Global::client.available());  // wait for reponse
		if(first_line_is_invalid()) return clear_buffer_and_return_false();
		// ignore header info and get content length
		if(message_length() != PACKET_LENGTH) return clear_buffer_and_return_false();

		// FROM: https://en.wikipedia.org/wiki/HTTP_message_body
		//  The content length's last char & message body's first char will have 2 new lines between them.
		//  message_length() only consumes a new-line if the number of digits of PACKET_LENGTH do no match buffer.
		//  Therefore, for a valid packet, there should be 2 new lines then the message.

		// while not two consecutive new-lines, ignore left-over headers if able to
		while(Global::client.available() >= 2 && (Global::client.read() != '\n' || Global::client.read() != '\n'));
		if(Global::client.available() != PACKET_LENGTH) return false;

		Global::client.read(packet_buffer, PACKET_LENGTH);  // get packet (finally)

		return return_whether_buffer_is_empty_and_clear_it_if_not();  // should always be true, but let's be prudent :D
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
		return buffer_matches_string(compare_string, string_length(compare_string));
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
		return buffer_mismatches_string(compare_string, string_length(compare_string));
	}


	// Determine whether data sent is valid by performing a checksum on all bytes except last one (checksum byte).
	// Takes the location of the packet_array.
	// Iterates string (except the last one), X-ORing each byte with the previous ones.
	// Returns whether calculated and sent checksums match.
	bool checksum_packet(byte packet_buffer[])
	{
		byte checksum = 0;
		for(uint8_t x = 0; x < PACKET_LENGTH-1; x++) checksum ^= packet_buffer[x];
		return checksum == packet_buffer[CHECKSUM];
	}


	// SUGAR: Clears the buffer of remaining character & returns false.
	// Iterates through buffer until empty.
	// Returns false (so that calling function can return it (and false)).
	bool clear_buffer_and_return_false()
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


	// THIS DOES NOT SAVE THE READ DATA & COULD CONSUME AN EXTRA NEWLINE IF CONTENT LENGTH < 10
	// Gets the content length of the message.
	// Reads header (or all) in until Content-Lenght string is found.  Then gets the next three bytes/numbers unless a 
	// new-line is encountered.  Then converts the read in bytes to a uint8_t.
	// Returns length of the message if parsed properly, otherwise 0.
	uint8_t message_length()
	{
		// skip while at wrong part
		while(Global::client.available() && buffer_mismatches_string(CONTENT_LENGTH_CONST_CSTR));

		char number_buffer[3] = {0, 0, 0};  // zero out for arbitrary number between [0, 99]
		// (get next three bytes OR stop at newline) if buffer available (should be, but double check :) )
		for(uint8_t x = 0; x < 2 && (x == 0 || number_buffer[x-1] != '\n') && Global::client.available(); x++)
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
		return atoi(number_buffer);
	}


	// Compare first line of response with known response to see if successful.
	// Uses defined string to compare each character of response for accuracty.
	// Returns if they do not match.
	bool first_line_is_invalid()
	{
		return buffer_matches_string(VALID_RESPONSE_STR, string_length(VALID_RESPONSE_STR));
	}


	// SUGAR: Name should explain what it does.
	// Returns true if buffer is empty, else false if not empty.
	bool return_whether_buffer_is_empty_and_clear_it_if_not()
	{
		if(!Global::client.available()) return true;
		return clear_buffer_and_return_false();
	}


	// LENGTH DOES NOT INCLUDE Null terminator.
	// The old tried and test string with the new twist of a better name. ;)
	// Takes a byte array (that is hopefully Null Terminated).
	// Iterates array until Null terminator is found or max length is reached.
	// Return length of string (or max uint8_t).
	uint8_t string_length(const char string[])
	{
		uint8_t length = 255;
		while(length && string[255-length]) length--;
		return 255-length;
	}

}  // end namespace Transmission

#endif
