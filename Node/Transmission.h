
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.06.06                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#pragma once


#include <HttpClient.h>

#include "Global.h"
#include "User.h"


namespace Transmission
{
	// ————————————————————————————————————————————— TRANSMISSION::GLOBAL —————————————————————————————————————————————

	// ———— STRING LITERALS ————
	// ———— HTTP LITERALS ————
	const uint8_t VALID_REQUEST_STR[] = "HTTP/1.1 200 OK\r\n";  // start string for valid request from device
	const uint8_t INVALID_REQUEST_STR[] = "HTTP/1.1 400 Bad Request\r\n";  // start string for invalid request from device
	// ———— HTTP LITERALS::REQUEST TYPES ————
	const uint8_t POST[] = "POST ";
	const uint8_t HTTP_VERSION[] = " HTTP/1.1\r\n";
	// ———— HTTP LITERALS::HEADER INFO ————
	const uint8_t CONTENT_TYPE[] = "Content-Type: application/json\r\n";
	const char CONTENT_TYPE_TAG[] = "Content-Type";
	const char CONTENT_TYPE_VALUE[] = "application/json";
	const uint8_t CONTENT_LENGTH_TAG[] = "Content-Length: ";
	// ———— HTTP LITERALS::HEADER INFO ————
	const uint8_t CARRIAGE_NEW_LINE[] = "\r\n";  // since I always forget the order
	const uint8_t DOUBLE_CARRIAGE_NEW_LINE[] = "\r\n\r\n";  // since I always forget the order

	// ———— ENCODING ————
	// Designed for:
	//	{"event" : 4294967295, "length" : 4294967295, "current position" : 4294967295, "desired position" : 4294967295,
	//	  "direction" : 1, "auto calibrate" : 1, "auto correct" : 1}
	// — OR —
	//	{"event":0,"length":0,"current position":0,"desired position":0,"direction":1,"auto calibrate":1,
	//	  "auto correct":1}
	
	const uint8_t MAX_PACKET_LENGTH = 188;  // should be a max of 188(189 w/ '\0') chars
	const uint8_t MIN_PACKET_LENGTH = 104;  // every valid packet received will have at least this amount of chars
	// json
	const char CURTAIN_KEY[] = "curtain";
	const char CURRENT_POS_KEY[] = "current position";
	const char LENGTH_KEY[] = "length";

	const char EVENT_KEY[] = "event";
	const char DESIRED_POS_KEY[] = "desired position";

	const char CALIBRATE_KEY[] = "auto calibrate";
	const char CORRECT_KEY[] = "auto correct";
	const char DIRECTION_KEY[] = "direction";
	// messages
	const uint8_t INVALID_RESPONSE[] = "{\"error\" : \"Package received does not match JSON format\"}\r\n";
	const uint8_t VALID_RESPONSE[] = "{\"success\":\"Valid JSON received\"}\r\n";

// —————————————————————————————————————————————————— RECEIVE DATA —————————————————————————————————————————————————— //

	// Skips the header read in from the client.
	// Reads through each character from client matching the character to the state machine to match to a double 
	//  carriage return-newline combo.
	// Return whether the body of the client is found.
	bool skip_header()
	{
		HARDWARE_CLIENT* client = &Global::client;  //SUGAR
		for(uint32_t x = 0; x < 0xFFFFFFFF && client->available() >= 4; x++)
		{
			// State machine
			if(client->read() == '\r' && client->read() == '\n' && client->read() == '\r' && client->read() == '\n')
			{
				return true;
			}
		}
		return false;
	}

	// SUMMARY:	Waits until client request received by the server, then converts and returns an HttpClient.
	// DETAILS:	Creates static variable to stay alive for reference by returned HttpClient. Stays in loop until a client
	//  is successfully returned by the server. Converts client to HttpClient.
	// RETURN:	HttpClient referncing client.
	HARDWARE_CLIENT wait_for_request()
	{
		HARDWARE_CLIENT client = Global::server.available();
		while(!client)
		{
			delayMicroseconds(10);
			client = Global::server.available();
		}

		return client;
	}


	// SUMMARY:	Reads the client request into a dynamically allocated buffer.
	// DETAILS:	Skips over the content headers. Allocates enough memory for content length and reads it in from client
	//  into buffer.
	// RETURN:	Populated buffer if successfully read, otherwise NULL pointer to indicate error occuring.
	char* read_transmission_data_into_buffer()
	{
		char* content = (char*)malloc(JSON_BUFFER_SIZE);

		uint16_t x;  //TRANSFER
		if(!content || !skip_header()) return (char*)NULL;
		for(x = 0; Global::client.available(); x++)
		{
			content[x] = Global::client.read();
		}
		content[x] = 0;  //TRANSFER
		if(JSON_BUFFER_SIZE < x)
		{
			delete content;
			return (char*)NULL;
		}

		return content;
	}


// ——————————————————————————————————————————————————— SEND  DATA ——————————————————————————————————————————————————— //

	// SUMMARY:	Writes the post request to the client adding headers to imply JSON.
	// PARAMS:	Takes the JSON string to write to send, the client's path to send to.
	// DETAILS:	
	// void post_json(char json[])
	void post_json(char json[], const char path[])
	{
		Global::client.println("POST /api/update/deactivateevent HTTP/1.1");
		Global::client.println("Host: 10.0.0.23");
		Global::client.println("Content-Type: application/json");
		Global::client.print((const char*)CONTENT_LENGTH_TAG);
		Global::client.println(C_String::length(json));
		Global::client.println();
		Global::client.print(json);
	}


	// Request received from Hub. Sends response indicating request was invalid.
	void send_invalid_response_and_delete_(char json_buffer[])
	{
		delete json_buffer;  // put here to allow returning from Node::loop function

		Global::client.write(INVALID_REQUEST_STR, sizeof(INVALID_REQUEST_STR)-1);  //TRANSFER
		Global::client.write(CONTENT_TYPE, sizeof(CONTENT_TYPE)-1);  //TRANSFER
		// Content
		String content_length_string = String(sizeof(VALID_RESPONSE)-1);
		Global::client.write(CONTENT_LENGTH_TAG, sizeof(CONTENT_LENGTH_TAG)-1);
		Global::client.write((const uint8_t*)content_length_string.c_str(), content_length_string.length());  //TRANSFER
		Global::client.write(DOUBLE_CARRIAGE_NEW_LINE, 4);  //TRANSFER
		Global::client.write(INVALID_RESPONSE, sizeof(INVALID_RESPONSE)-1);

		Global::client.stop();
	}


	// Request received from Hub. Sends response indicating request was valid.
	void send_OK_response_and_stop_client()
	{
		Global::client.write((const uint8_t*)VALID_REQUEST_STR, sizeof(VALID_REQUEST_STR)-1);  //TRANSFER
		Global::client.write((const uint8_t*)CONTENT_TYPE, sizeof(CONTENT_TYPE)-1);  //TRANSFER
		// Content
		String content_length_string = String(sizeof(VALID_RESPONSE)-1);
		Global::client.write(CONTENT_LENGTH_TAG, sizeof(CONTENT_LENGTH_TAG)-1);
		Global::client.write((const uint8_t*)content_length_string.c_str(), content_length_string.length());  //TRANSFER
		Global::client.write(DOUBLE_CARRIAGE_NEW_LINE, 4);  //TRANSFER
		Global::client.write((const uint8_t*)VALID_RESPONSE, sizeof(VALID_RESPONSE)-1);

		Global::client.stop();
	}


	void update_hub(byte packet_buffer[])
	{
		if(Global::client.connected()) Global::client.stop();  // make sure I wasn't incompetent :)

		// Establish connection
		static HARDWARE_CLIENT client;
		Global::client = client;
		if(!Global::client.connect(User::hub_host_cstr, User::port)) return;

		// Send data if eventually connected
		uint8_t timeout;
		for(timeout = 255; !Global::client.connected() && timeout; timeout--) delay(10);
		if(timeout)
		{
			post_json((char*)packet_buffer, USER_COMPLETE_PAGE);
			while(Global::client.available()) Global::client.read();
			Global::client.stop();
		}
	}

} // end namespace Transmission
