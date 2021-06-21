
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
	
	// ———— ENCODING ————
	// Designed for:
	//	{"event" : 4294967295, "length" : 4294967295, "current position" : 4294967295, "desired position" : 4294967295,
	//	  "direction" : 1, "auto calibrate" : 1, "auto correct" : 1}
	// — OR —
	//	{"event" : 0, "length":0,"current position":0,"desired position":0,"direction":1,"auto calibrate":1,
	//	  "auto correct":1}
	
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
	const char INVALID_JSON[] = "{\"error\" : \"Package received does not match JSON format\"}";
	const char VALID_RESPONSE[] = "{\"success\":\"Valid JSON received\"}";


	// SUMMARY:	Waits until client request received by the server, then converts and returns an HttpClient.
	// DETAILS:	Creates static variable to stay alive for reference by returned HttpClient. Stays in loop until a client
	//  is successfully returned by the server. Converts client to HttpClient.
	// RETURN:	HttpClient referncing client.
	HttpClient* wait_for_request()
	{
		static Client client = Global::server.available();
		while(!client)
		{
			delayMicroseconds(10);
			client = Global::server.available();
		}
		return new HttpClient(client);
	}


	// SUMMARY:	Writes the post request to the client adding headers to imply JSON.
	// PARAMS:	Takes the JSON string to write to send, the client's path to send to.
	// DETAILS:	
	void post_json(char json[])
	// void post_json(char json[], const char path[])
	{
		// Global::client->sendHeader("Content-Type", "application/json");
		// Global::client->sendHeader("Content-Length", C_String::length(json));

		// Global::client->post(User::hub_host_cstr, path);
		Global::client->write((const uint8_t*)json, C_String::length(json));
	}


	// SUMMARY:	Reads the client request into a dynamically allocated buffer.
	// DETAILS:	Skips over the content headers. Allocates enough memory for content length and reads it in from client
	//  into buffer.
	// RETURN:	Populated buffer if successfully read, otherwise NULL pointer to indicate error occuring.
	char* read_transmission_data_into_buffer()
	{
		if(!Global::client->skipResponseHeaders()) return (char*)NULL;

		uint32_t content_length = Global::client->contentLength();
		char* content = (char*)malloc(content_length+1);

		if(!content || (uint32_t)0xFF < content_length) return NULL;
		for(uint8_t x = 0; x < content_length && Global::client->available(); x++)
		{
			content[x] = Global::client->read();
		}

		return content;
	}


	void send_invalid_response_and_delete_(char json_buffer)
	{
		Global::client->write(INVALID_JSON, C_String::length(INVALID_JSON));
		Global::client->stop();
		delete json_buffer;
	}


	void send_OK_response_and_stop_client()
	{
		Global::client->write(VALID_RESPONSE, C_String::length(VALID_RESPONSE));
		Global::client->stop();
	}


	void update_hub(byte packet_buffer[])
	{
#ifdef __ETHERNET__
		EthernetClient connection_client;
#elif __WIFI__
		WiFiClient connection_client;
#endif
		connection_client.connect(User::hub_host, 80);  //HARDCODED: port
		if(Global::client->connected()) Global::client->stop();  // make sure I wasn't incompetent :)
		Global::client = new HttpClient(connection_client);

		uint8_t timeout = 255;
		for(timeout = 255; !client.connected() && timeout; timeout--) delayMicroseconds(10);

		if(timeout)
		{
			post_data((char*)packet_buffer, COMPLETE_PAGE_HEADER);
			clear_buffer();
		}

		Global::client->stop();
		delete Global::client;
	}
}