
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

#pragma once


#include<Ethernet.h>

#include "Global.h"


namespace Transmission
{
	// ———— SENDING/RECEIVING ————
	void post_data(char[], const char[]);
	void post_data(String, const char[]);
	bool request_successfully_read_into_(byte[]);
	void clear_client_and_send_invalid_json_response();
	void send_valid_json_response_and_stop_client();
	// ———— UTILITY ————
	bool buffer_matches_string(const char[], uint8_t);
	bool buffer_matches_string(const char[]);
	byte buffer_mismatches_string(const char[], uint8_t);
	byte buffer_mismatches_string(const char[]);
	bool checksum_packet(byte[]);
	void clear_buffer();
	bool clear_buffer_send_error_and_return_false(const char[]);
	bool clear_buffer_and_return_true();
	bool client_read_double_carriage_return_new_line();
	bool response_type_is_invalid();
	uint8_t message_length();
	uint64_t message_length(uint8_t);
	void update_hub(byte[]);
	WiFiClient wait_for_request();


	// ————————————————————————————————————————————— TRANSMISSION: GLOBAL —————————————————————————————————————————————

	// ———— STRING LITERALS ————
	const char CONTENT_LENGTH_CONST_CSTR[] = "Content-Length: ";
	const char JSON_HEADER[] = "application/x-www-form-urlencoded";
	const char COMPLETE_PAGE_HEADER[] = "POST " USER_COMPLETE_PAGE " HTTP/1.1";
	const char VALID_RESPONSE_STR[] = "HTTP/1.1 200 OK";  // initial string for valid response from device

	// ———— ENCODING ————
	// Designed for:
	//	{"event" : 4294967295, "length" : 4294967295, "current position" : 4294967295, "desired position" : 4294967295,
	//	  "direction" : 1, "auto calibrate" : 1, "auto correct" : 1}
	// — OR —
	//	{"event" : 0, "length":0,"current position":0,"desired position":0,"direction":1,"auto calibrate":1,
	//	  "auto correct":1}

	// if MIN_PACKET_LENGTH ever changes, change Transmissions::message_length(.) to match number of digits
	const uint8_t MIN_PACKET_LENGTH = 104;  // every valid packet received will have at least this amount of chars
	const uint8_t BUFFER_LENGTH = 255;  // should be a max of 188(189) chars

	// json
	const char CURTAIN_KEY[] = "\"curtain\"";
	const char CURRENT_POS_KEY[] = "\"current position\"";
	const char LENGTH_KEY[] = "\"length\"";

	const char EVENT_KEY[] = "\"event\"";
	const char DESIRED_POS_KEY[] = "\"desired position\"";

	const char CALIBRATE_KEY[] = "\"auto calibrate\"";
	const char CORRECT_KEY[] = "\"auto correct\"";
	const char DIRECTION_KEY[] = "\"direction\"";

	// messages
	const char BAD_JSON[] = "{\"error\" : \"Package received does not match JSON format\"}";
	const char BAD_RESPONSE_CODE[] = "{\"error\" : \"Invalid response code\"}";
	const char INVALID_MAX_LENGTH[] = "{\"error\" : \"Packet length too long\"}";
	const char INVALID_MIN_LENGTH[] = "{\"error\" : \"Packet length too short\"}";
	const char LEFT_OVER_CHARACTERS[] = "{\"error\" : \"Not all characters were consumed from buffer\"}";
	const char NO_DATA[] = "{\"error\" : \"Too many characters consumed searching for double newline\"}";
	const char VALID_RESPONSE[] = "{\"success\":\"Valid JSON received\"}";


	// ——————————————————————————————————————————————— SENDING/RECEIVING ———————————————————————————————————————————————

	// Sends data using POST method to HOST.
	// Takes char array of data to post.  Prints data to client.
	// Prints to client as per https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/POST
	void post_data(char data[], const char header[]=VALID_RESPONSE_STR)
	{
		Global::client->println(header);

		Global::client->print("Host: ");
		Global::client->println(User::hub_host_cstr);

		Global::client->println("Content-Type: application/json");
		Global::client->print("Content-Length: ");
		Global::client->println(C_String::length(data));
		Global::client->println();
		Global::client->print(data);
		Global::client->println();
	}


	// Sends data using POST method to HOST.
	// Takes char array of data to post.  Prints data to client.
	void post_data(String data, const char header[]=VALID_RESPONSE_STR)
	{
		post_data(data.c_str(), header);
	}


	// Clears the client buffer, post message to client, & stops client.
	// Takes pointer to message c-string.
	void clear_post_and_end_client(const char message[])
	{
		clear_buffer();
		post_data((char*)message);
		Global::client->stop();
	}



	bool send_message_and_end_client(const char message[])
	{
		clear_post_and_end_client(message);
		return false;
	}


	// Reads the incoming transmission from the response.
	// Takes pointer to the buffer where the scraped data will be stored.
	// Checks that the data transmission is of the correct format. Sends error response if not of correct format.
	// Returns whether data packet of correct format.
	bool request_successfully_read_into_(byte packet_buffer[])
	{
		while(!Global::client->available());  // wait for reponse
		if(response_type_is_invalid()) return send_message_and_end_client(BAD_RESPONSE_CODE);

		uint8_t length = message_length();  // ignore header & get content length
		if(length < MIN_PACKET_LENGTH) return send_message_and_end_client(INVALID_MIN_LENGTH);
		if(BUFFER_LENGTH <= length) return send_message_and_end_client(INVALID_MAX_LENGTH);

		// FROM: https://en.wikipedia.org/wiki/HTTP_message_body
		//  The content length's last char & message body's first char will have 2 new lines between them.
		//  message_length() only consumes a new-line if the number of digits of PACKET_LENGTH do no match buffer.
		//  Therefore, for a valid packet, there should be 2 new lines then the message.

		// while not two consecutive new-lines, ignore left-over headers if able to
		// message_length() should end before eats up \n. If it doesn't, something is wrong & rest is ignored
		while(Global::client->available() >= 2 && !client_read_double_carriage_return_new_line());
		if(Global::client->available() < MIN_PACKET_LENGTH) return send_message_and_end_client(NO_DATA);

		int x;
		for(x = 0; Global::client->available() && x < BUFFER_LENGTH; x++) packet_buffer[x] = Global::client->read();

		// should always be empty and return !false, but let's be prudent :D
		if(Global::client->available() || x == BUFFER_LENGTH) return send_message_and_end_client(LEFT_OVER_CHARACTERS);

		packet_buffer[x] = 0;
		return true;
	}


	void send_valid_json_response_and_stop_client()
	{
		Transmission::post_data((char*)VALID_RESPONSE);
		Global::client->stop();
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	// THIS DOES NOT SAVE THE READ DATA
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with, the length of said string.
	// Iterates through C-string, checking that each byte matches.
	// Returns true if string matches buffer, false otherwise.
	bool buffer_matches_string(const char compare_string[], uint8_t length)
	{
		if(Global::client->available() < length) return false;  // not != b/c might want to pull more after match
		for(uint8_t x = 0; x < length; x++) if(compare_string[x] != Global::client->read()) return false;

		return true;
	}


	// THIS DOES NOT SAVE THE READ DATA
	// Read in buffer at current position and compare to string.
	// Takes a C string to compare read data with.
	// Determines the length of the string. Iterates through C-string, checking that each byte matches.
	// Returns true if string matches buffer, false otherwise.
	bool buffer_matches_string(const char compare_string[])
	{
		return buffer_matches_string(compare_string, C_String::length((char*)compare_string));
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
		for(x = 0; Global::client->available() && x < length; x++)
		{
			previous = Global::client->read();
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
		return buffer_mismatches_string(compare_string, C_String::length((char*)compare_string));
	}


	// Discards info from client while it is available.
	void clear_buffer()
	{
		while(Global::client->available()) Global::client->read();
	}


	// SUGAR: Clears the buffer of remaining character & returns false.
	// Iterates through buffer until empty.
	// Returns false (so that calling function can return it (and false)).
	bool clear_buffer_send_error_and_return_false(const char message[])
	{
		clear_buffer();
		post_data(message);
		return false;
	}


	// SUGAR: Clears the buffer of remaining character & returns true.
	// Iterates through buffer until empty.
	// Returns true (so that calling function can return it (and true)).
	bool clear_buffer_and_return_true()
	{
		while(Global::client->available()) Global::client->read();
		return true;
	}


	// For those who like to smother protocols with \r
	bool client_read_double_carriage_return_new_line()
	{
		WiFiClient* client = Global::client;
		return client->read() == '\r' && client->read() == '\n' && client->read() == '\r' && client->read() == '\n';
	}


	// Compare first line of response with known response to see if successful.
	// Uses defined string to compare each character of response for accuracty.
	// Returns if they do not match.
	bool response_type_is_invalid()
	{
		return buffer_matches_string(VALID_RESPONSE_STR, C_String::length((char*)VALID_RESPONSE_STR));
	}


	// THIS DOES NOT SAVE THE READ DATA & COULD CONSUME AN EXTRA NEWLINE IF CONTENT LENGTH < 100
	// Gets the content length of the message.
	// Reads header (or all) in until Content-Lenght string is found.  Then gets the next three bytes/numbers unless a 
	// new-line is encountered.  Then converts the read in bytes to a uint8_t.
	// Returns length of the message if parsed properly, otherwise 0.
	uint8_t message_length()
	{
		// skip while at wrong part
		while(Global::client->available() && buffer_mismatches_string(CONTENT_LENGTH_CONST_CSTR));

		char number_buffer[4] = {0, 0, 0, 0};  // zero out for arbitrary number between [0, 999]
		// (get next three bytes OR stop at newline) if buffer available (should be, but double check :) )
		for(uint8_t x = 0; x < 3 && (x == 0 || number_buffer[x-1] != '\n') && Global::client->available(); x++)
		{
			number_buffer[x] = Global::client->read();
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
		while(Global::client->available() && buffer_mismatches_string(CONTENT_LENGTH_CONST_CSTR));

		// zero out for arbitrary number between [0, 10^max_digits]
		char* number_buffer = (char*)calloc(max_digits+1, sizeof(char));
		// (get next three bytes OR stop at newline) if buffer available (should be, but double check :) )
		for(uint8_t x = 0; x < max_digits && (x == 0 || number_buffer[x-1] != '\n') && Global::client->available(); x++)
		{
			number_buffer[x] = Global::client->read();
		}

		uint64_t length = atoi(number_buffer);
		free(number_buffer);  // didn't forget to do this ;)

		return length;
	}


	void update_hub(byte packet_buffer[])
	{
		WiFiClient client;
		client.connect(User::hub_host, 80);  //HARDCODED: port

		if(Global::client->connected()) Global::client->stop();  // make sure I wasn't incompetent :)
		Global::client = &client;

		uint8_t timeout = 255;
		for(timeout = 255; !client.connected() && timeout; timeout--) delayMicroseconds(10);

		if(timeout)
		{
			post_data((char*)packet_buffer, COMPLETE_PAGE_HEADER);
			clear_buffer();
		}

		client.stop();
		Global::client = NULL;
	}


	WiFiClient wait_for_request()
	{
		while(!Global::server.available()) delayMicroseconds(10);
		return Global::server.available();
	}

}  // end namespace Transmission
