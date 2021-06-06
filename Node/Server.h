
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


#include "Global.h"


namespace Transmission
{
	// ————————————————————————————————————————————— TRANSMISSION::GLOBAL —————————————————————————————————————————————

	// ———— STRING LITERALS ————
	const char VALID_RESPONSE_STR[] = "HTTP/1.1 200 OK";  // initial string for valid response from device


	// SUMMARY:	Waits until client request received by the server, then converts and returns an HttpClient.
	// DETAILS:	Creates static variable to stay alive for reference by returned HttpClient. Stays in loop until a client
	//  is successfully returned by the server. Converts client to HttpClient.
	// RETURN:	HttpClient referncing client.
	HttpClient wait_for_request()
	{
		static Client client = Global::server.available();
		while(!client)
		{
			delayMicroseconds(10);
			client = Global::server.available();
		}
		return HttpClient(client);
	}


	// SUMMARY:	Reads the client request into a dynamically allocated buffer.
	// DETAILS:	Skips over the content headers. Allocates enough memory for content length and reads it in from client
	//  into buffer.
	// RETURN:	Populated buffer if successfully read, otherwise NULL pointer to indicate error occuring.
	char* read_transmission_data_into_buffer()
	{
		if(!Global::client.skipResponseHeaders()) return (char*)NULL;

		uint32_t content_length = Global::client.contentLength();
		char* content = (char*)malloc(content_length+1);

		if(!content || (uint32_t)0xFF < content_length) return NULL;
		for(uint8_t x = 0; x < content_length && Global::client.available(); x++)
		{
			content[x] = Global::client.read();
		}

		return content;
	}


	// SUMMARY:	Writes the post request to the client adding headers to imply JSON.
	// PARAMS:	Takes the JSON string to write to send, the client's path to send to.
	// DETAILS:	
	void post_json(char json[])
	// void post_json(char json[], const char path[])
	{
		// Global::client.sendHeader("Content-Type", "application/json");
		// Global::client.sendHeader("Content-Length", C_String::length(json));

		// Global::client.post(User::hub_host_cstr, path);
		Global::client.write((const uint8_t*)json, C_String::length(json));
	}


	void send_OK_response_and_stop_client()
	{
		Global::client.write(VALID_RESPONSE, C_String::length(VALID_RESPONSE));
		Global::client.stop();
	}
}