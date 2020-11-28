
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

#ifndef _Transmissions_
#define _Transmissions_


#include "Global.h"


namespace Transmissions
{

	const uint8_t PACKET_LENGTH = 10;  // if this ever changes, change Transmissions::message_length(.) to match number of digits
	const char VALID_RESPONSE_STR[] = "HTTP/1.1 200 OK";  // initial string for valid response from device

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


	// ———————————————————————————————————————————————— READING/WRITING ————————————————————————————————————————————————

	// Reads base128 data points from packet byte array.
	// Takes the location of the packet array.
	// Substracts the added 1 from each byte. Bit shifts each part of the base128 number to its corresponding part in 
	// the uint32_t number. Places segment into number. Stores numbers into Transmision struct.
	// Returns Transmission struct of decoded data.
	Curtain decode_response()
	{
		Curtain curtain = {0, 0, 0, 0, 0, 0};
		curtain.calibrate = PACKET_BUFFER[OPTIONS] & CALIBRATE;
		curtain.correct = PACKET_BUFFER[OPTIONS] & CORRECT;
		curtain.direction = PACKET_BUFFER[OPTIONS] & DIRECTION;

		curtain.current_position =	PACKET_BUFFER[CURRENT_UP] - 1 << 14
									  | PACKET_BUFFER[CURRENT_MID] - 1 << 7
									  | PACKET_BUFFER[CURRENT_LOW] - 1;

		curtain.desired_position =	(PACKET_BUFFER[DESIRED_UP] - 1 << 14)
									  | (PACKET_BUFFER[DESIRED_MID] - 1 << 7)
									  | PACKET_BUFFER[DESIRED_LOW] - 1;

		curtain.length =	(PACKET_BUFFER[LENGTH_UP] - 1 << 14)
							  | (PACKET_BUFFER[LENGTH_MID] - 1 << 7)
							  | PACKET_BUFFER[LENGTH_LOW] - 1;

		return curtain;
	}


	// Writes relevent datat to packet array.
	// Takes location of packet array, the location of curtain (to prevent needing to copy).
	// Sets current position and length in base 128 + 1 equivalent values.
	void encode_curtain(Curtain& curtain)
	{
		for(int x = 0; x < PACKET_LENGTH; x++) PACKET_BUFFER[x] = 1;  // reset PACKET_BUFFER

		PACKET_BUFFER[CURRENT_LOW] = (curtain.current_position & 0x7F) + 1;
		PACKET_BUFFER[CURRENT_MID] = (curtain.current_position >> 7 & 0x7F) + 1;
		PACKET_BUFFER[CURRENT_UP] = (curtain.current_position >> 14 & 0x7F) + 1;

		PACKET_BUFFER[LENGTH_LOW] = (curtain.length & 0x7F) + 1;
		PACKET_BUFFER[LENGTH_MID] = (curtain.length >> 7 & 0x7F) + 1;
		PACKET_BUFFER[LENGTH_UP] = (curtain.length >> 14 & 0x7F) + 1;
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
	void post_data(char data[])
	{
		ensure_connection();

		CLIENT.println("POST " PAGE " HTTP/1.1");
		CLIENT.println("Host: " MASTER_HOST_STR);
		CLIENT.println("Content-Type: application/x-www-form-urlencoded");
		CLIENT.print("Content-Length: ");
		CLIENT.println(string_length(data));
		CLIENT.println();
		CLIENT.print(data);
		CLIENT.println();
	}


	// Reads the incoming transmission from the response.
	// Takes pointer to the buffer where the scraped data will be stored.
	// Checks that the data transmission is of the correct format.
	// Returns true if valid. 
	bool read_state_response_successfully_into_buffer()
	{
		while(!CLIENT.available());  // wait for reponse
		if(first_line_is_invalid()) return clear_buffer_and_return_false();
		// ignore header info and get content length
		if(message_length() != PACKET_LENGTH) return clear_buffer_and_return_false();

		// FROM: https://en.wikipedia.org/wiki/HTTP_message_body
		//  The content length's last char & message body's first char will have 2 new lines between them.
		//  message_length() only consumes a new-line if the number of digits of PACKET_LENGTH do no match buffer.
		//  Therefore, for a valid packet, there should be 2 new lines then the message.

		// while not two consecutive new-lines, ignore left-over headers if able to
		while(CLIENT.available() >= 2 && (CLIENT.read() != '\n' || CLIENT.read() != '\n'));
		if(CLIENT.available() != PACKET_LENGTH) return false;

		CLIENT.read(PACKET_BUFFER, PACKET_LENGTH);  // get packet (finally)

		return return_whether_buffer_is_empty_and_clear_it_if_not();  // should always be true, but let's be prudent :D
	}



	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	// THIS DOES NOT SAVE THE READ DATA
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with, the length of said string.
	// Iterates through C-string, checking that each byte matches.
	// Returns true string matches buffer, false otherwise.
	bool buffer_matches_string(byte compare_string[], uint8_t string_length)
	{
		if(CLIENT.available() < string_length) return false;  // not != b/c might want to pull more data after match
		for(uint8_t x = 0; x < string_length; x++)
		{
			previous = CLIENT.read();
			if(compare_string[x] != previous) return false;
		}
		return true;
	}


	// THIS DOES NOT SAVE THE READ DATA
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with.
	// Determines the length of the string. Iterates through C-string, checking that each byte matches.
	// Returns true string matches buffer, false otherwise.
	bool buffer_matches_string(byte compare_string[])
	{
		return buffer_matches_string(compare_string, string_length(compare_string));
	}


	// THIS ONLY RETURNS THE LAST CHARACTER READ THAT DOES NOT MATCH STRING
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with, the length of said string.
	// Iterates through C-string, checking that each byte matches.
	// Returns NULL (no mismatch) if the string matches the buffer, ELSE the last seen character, if mismatch or 1 if 
	// read is unavailable.
	byte buffer_mismatches_string(byte compare_string[], uint8_t string_length)
	{
		uint8_t x;
		for(x = 0; CLIENT.available() && x < string_length; x++)
		{
			previous = CLIENT.read();
			if(compare_string[x] != previous) return previous;
		}
		return x != string_length;  // return NULL if x is the length of string, 1 if x is not length of string
	}


	// THIS ONLY RETURNS THE LAST CHARACTER READ THAT DOES NOT MATCH STRING
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with.
	// Determines the length of the string. Iterates through C-string, checking that each byte matches.
	// Returns NULL (no mismatch) if the string matches the buffer, ELSE the last seen character, if mismatch or 1 if
	// read is unavailable.
	byte buffer_mismatches_string(byte compare_string[])
	{
		return buffer_mismatches_string(compare_string, string_length(compare_string));
	}


	// Determine whether data sent is valid by performing a checksum on all bytes except last one (checksum byte).
	// Takes the location of the packet_array.
	// Iterates string (except the last one), X-ORing each byte with the previous ones.
	// Returns whether calculated and sent checksums match.
	bool checksum_packet()
	{
		byte checksum = 0;
		for(uint8_t x = 0; x < PACKET_LENGTH-1; x++) checksum ^= PACKET_BUFFER[x];
		return checksum == PACKET_BUFFER[CHECKSUM];
	}


	// SUGAR: Clears the buffer of remaining character & returns false.
	// Iterates through buffer until empty.
	// Returns false (so that calling function can return it (and false)).
	bool clear_buffer_and_return_false()
	{
		while(CLIENT.available()) CLIENT.read();
		return false;
	}


	// SUGAR: Clears the buffer of remaining character & returns true.
	// Iterates through buffer until empty.
	// Returns true (so that calling function can return it (and true)).
	bool clear_buffer_and_return_true()
	{
		while(CLIENT.available()) CLIENT.read();
		return true;
	}


	// THIS DOES NOT SAVE THE READ DATA & COULD CONSUME AN EXTRA NEWLINE IF CONTENT LENGTH < 10
	// Gets the content length of the message.
	// Reads header (or all) in until Content-Lenght string is found.  Then gets the next three bytes/numbers unless a 
	// new-line is encountered.  Then converts the read in bytes to a uint8_t.
	// Returns length of the message if parsed properly, otherwise 0.
	uint8_t message_length()
	{
		while(CLIENT.available() && buffer_mismatches_string("Content-Length: "));  // skip while not at right part

		byte number_buffer[3] = {0, 0, 0};  // zero out for arbitrary number between [0, 99]
		// (get next three bytes OR stop at newline) if buffer available (should be, but double check :) )
		for(uint8_t x = 0; x < 2 && (x == 0 || number_buffer[x-1] != '\n') && CLIENT.available(); x++)
		{
			number_buffer[x] = CLIENT.read();
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
		while(CLIENT.available() && buffer_mismatches_string("Content-Length: "));  // skip while not at right part

		// zero out for arbitrary number between [0, 10^max_digits]
		byte* number_buffer = calloc(max_digits+1, sizeof(byte));
		// (get next three bytes OR stop at newline) if buffer available (should be, but double check :) )
		for(uint8_t x = 0; x < max_digits && (x == 0 || number_buffer[x-1] != '\n') && CLIENT.available(); x++)
		{
			number_buffer[x] = CLIENT.read();
		}
		return atoi(number_buffer);
	}


	// Checks whether server is connected.
	// If not, continually tries to connect to server.
	void ensure_connection()
	{
		while(!CLIENT.connected()) 
		{
			#ifdef _TESTING_
				Serial.println(DEF(__LINE__) "No connection");
			#endif
			delay(1000);
			CLIENT.connect(SERVER, 80);
		}
	}


	// Compare first line of response with known response to see if successful.
	// Uses defined string to compare each character of response for accuracty.
	// Returns if they do not match.
	bool first_line_is_invalid()
	{
		return buffer_matches_string(VALID_RESPONSE_STR, sizeof(VALID_RESPONSE_STR)-1);
	}


	// SUGAR: Name should explain what it does.
	// Returns true if buffer is empty, else false if not empty.
	bool return_whether_buffer_is_empty_and_clear_it_if_not()
	{
		if(!CLIENT.available()) return true;
		return clear_buffer_and_return_false();
	}


	// LENGTH DOES NOT INCLUDE Null terminator.
	// The old tried and test string with the new twist of a better name. ;)
	// Takes a byte array (that is hopefully Null Terminated).
	// Iterates array until Null terminator is found or max length is reached.
	// Return length of string (or max uint8_t).
	uint8_t string_length(byte string)
	{
		uint8_t length = 255;
		while(length && string[255-length]) length--;
		return 255-length;
	}

}  // end namespace Transmission

#endif
