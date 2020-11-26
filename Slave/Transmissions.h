
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.25                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Read and write functions for communication between Raspberry Pi and Arduino. Second iteration for     *
*    comms functions for current curtain version.  Transmissions are sent adding 1 to each number to prevent a null    *
*    terminator from being sent—I've had problems with this in the past.                                               *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/

#ifndef _Transmissions_
#define _Transmissions_


#include "Global.h"

#define OPTIONS 0  // location in transmission of options bits
#define CURRENT_LOW 1  // location in transmission of lower 7 bits of current known curtain position
#define CURRENT_MID 2  // location in transmission of middle 7 bits of current known curtain position
#define CURRENT_UP 3  // location in transmission of upper 7 bits of current known curtain position
#define LENGTH_LOW 4  // location in transmission of lower 7 bits of curtain length
#define LENGTH_MID 5  // location in transmission of middle 7 bits of curtain length
#define LENGTH_UP 6  // location in transmission of upper 7 bits of curtain length
#define DESIRED_LOW 7  // location in transmission of lower 7 bits of desired curtain position
#define DESIRED_MID 8  // location in transmission of middle 7 bits of desired curtain position
#define DESIRED_UP 9  // location in transmission of upper 7 bits of desired curtain position
#define CHECKSUM 10  // always has to be a the end (see checksum_packet())


// —————————————————————————————————————————————————— READING/WRITING ——————————————————————————————————————————————————

// Reads base128 data points from packet byte array.
// Takes the location of the packet array.
// Substracts the added 1 from each byte. Bit shifts each part of the base128 number to its corresponding part in the
// uint32_t number. Places segment into number. Stores numbers into Transmision struct.
// Returns Transmission struct of decoded data.
Transmission decode_transmission(byte packet[])
{
	Transmission transmission = {0, 0, 0, 0};
	transmission.options = packet[OPTIONS];

	transmission.current_position =	packet[CURRENT_UP] - 1 << 14
									| packet[CURRENT_MID] - 1 << 7
									| packet[CURRENT_LOW] - 1;

	transmission.desired_position =	(packet[DESIRED_UP] - 1 << 14)
									| (packet[DESIRED_MID] - 1 << 7)
									| packet[DESIRED_LOW] - 1;

	transmission.length =	(packet[LENGTH_UP] - 1 << 14)
							| (packet[LENGTH_MID] - 1 << 7)
							| packet[LENGTH_LOW] - 1;

	return transmission;
}


// Writes relevent datat to packet array.
// Takes location of packet array, the current position, and calculated length (or last known length).
// Sets current position and length in base 128 + 1 equivalent values.
void encode_response(byte packet[], uint32_t current_position, uint32_t length)
{
	for(int x = 0; x < PACKET_LENGTH; x++) packet[x] = 1;  // reset packet

	packet[CURRENT_LOW] = (current_position & 0x7F) + 1;
	packet[CURRENT_MID] = (current_position >> 7 & 0x7F) + 1;
	packet[CURRENT_UP] = (current_position >> 14 & 0x7F) + 1;

	packet[LENGTH_LOW] = (length & 0x7F) + 1;
	packet[LENGTH_MID] = (length >> 7 & 0x7F) + 1;
	packet[LENGTH_UP] = (length >> 14 & 0x7F) + 1;
}


// ————————————————————————————————————————————————— SENDING/RECEIVING —————————————————————————————————————————————————

// Sends data using POST method to HOST.
// Takes char array of data to post.  Prints data to client.
void post_data(String data)
{
	ensure_connection();

	CLIENT.println(STR_SPC_DEF_SPC_STR(POST, PAGE, HTTP/1.1));
	CLIENT.println(STR_SPC_DEF(Host:, HOST));
	CLIENT.println("Content-Type: application/x-www-form-urlencoded");
	CLIENT.print("Content-Length: ");
	CLIENT.println(data.length());
	CLIENT.println();
	CLIENT.print(data);
	CLIENT.println();
}


// Sends data using POST method to HOST.
// Takes char array of data to post.  Prints data to client.
void post_data(char data[])
{
	ensure_connection();

	CLIENT.println(STR_SPC_DEF_SPC_STR(POST, PAGE, HTTP/1.1));
	CLIENT.println(STR_SPC_DEF(Host:, HOST));
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
bool read_data(byte buffer[])
{
	while(!CLIENT.available());  // wait for reponse
	if(first_line_is_invalid()) return false;
	if(content_length() != PACKET_LENGTH) return false;  // ignore header info and get content length

	// ignore 1-2 filler newlines (but save previous, because we cannot peak ahead)
	byte previous = 0;
	while(CLIENT.available() && previous == '\n') previous = CLIENT.read();

	CLIENT.read(buffer+1, PACKET_LENGTH-1);  // get rest of packet

	while(CLIENT.available()) CLIENT.read();  // clear remaining characters

	return *buffer == '{' && buffer[BUFFER_SIZE-2] == '}' && buffer[BUFFER_SIZE-1] == '}';
}



// —————————————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————————————

// THIS DOES NOT SAVE THE READ DATA
// Read in buffer at current position and compare to string.
// Takes a C string to compare read data with, the length of said string.
// Iterates through C-string, checking that each byte matches.
// Returns true string matches buffer, false otherwise.
bool buffer_matches_string(byte compare_string[], uint8_t string_length)
{
	if(CLIENT.available() < string_length) return false;
	for(uint8_t x = 0; x < string_length; x++) if(compare_string[x] != CLIENT.read()) return false;
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


// Determine whether data sent is valid by performing a checksum on all bytes except last one (checksum byte).
// Takes the location of the packet_array.
// Iterates string (except the last one), X-ORing each byte with the previous ones.
// Returns whether calculated and sent checksums match.
bool checksum_packet(byte packet[])
{
	byte checksum = 0;
	for(uint8_t x = 0; x < PACKET_LENGTH-1; x++) checksum ^= packet[x];
	return checksum == packet[CHECKSUM];
}


// THIS DOES NOT SAVE THE READ DATA
// Gets the content length of the message.
// Reads header (or all) in until Content-Lenght string is found.  Then gets the next three bytes/numbers unless a new-
// line is encountered.  Then converts the read in bytes to a uint8_t.
// Returns length of the message if parsed properly, otherwise 0.
uint16_t content_length()
{
	byte content_string[] = "Content-Length: "
	while(CLIENT.available())
	{
		while(CLIENT.read() != '\n');  // ignore line
		if(buffer_matches_string(content_string, sizeof(content_string)-1))
		{
			byte number_buffer[4] = {0, 0, 0, 0};  // maxes out at "999"
			// get next three bytes OR stop at newline
			for(uint8_t x = 0; (x == 0 || number_buffer[x-1] != '\n') && x < 3; x++) number_buffer[x] = CLIENT.read();
			return atoi(number_buffer);
		}
	}
	return 0;  // "Content-Length: " not found
}


// Checks whether server is connected.
// If not, continually tries to connect to server.
void ensure_connection()
{
	while(!CLIENT.connected()) 
	{
		#ifdef _TESTING_
			Serial.println(DEF_SPC_STR(__LINE__, No connection));
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
	char first_line[] = VALID_RESPONSE_STR;
	for(uint8_t x = 0; x < sizeof(first_line)-1; x++) if(first_line[x] != CLIENT.read()) return true;
	return false;
}


// The old tried and test string with the new twist of a better name. ;)
// Takes a byte array (that is hopefully Null Terminated).
// Iterates array until Null terminator is found or max length is reached.
// Return length of string (or max uint8_t).
uint8_t string_length(byte byte_string)
{
	uint8_t length = 255;
	while(length && compare_string[255-length]) length--;
	return 255-length;
}


#endif
