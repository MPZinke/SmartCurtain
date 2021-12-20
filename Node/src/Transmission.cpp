
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.10                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Transmission.hpp"


namespace Transmission
{	namespace Literal
	{
		namespace HTTP
		{
			// ———— START LINE ———— //
			const char VALID_REQUEST[] = "HTTP/1.1 200 OK";  // start string for valid request from device
			const char INVALID_REQUEST[] = "HTTP/1.1 400 Bad Request";  // start string for invalid request from device
			const char NO_CONTENT_REQUEST[] = "HTTP/1.1 204 No Content";  // start string for no content for request
			const char NOT_FOUND_REQUEST[] = "HTTP/1.1 404 Not Found";  // start string for no content for request
			// —— START LINE::POST —— //
			const char POST_METHOD[] = "POST ";
			const char HTTP_VERSION[] = " HTTP/1.1";

			// ———— HEADERS ———— //
			const char CONTENT_TYPE[] = "Content-Type: application/json";
			const char CONTENT_LENGTH_TAG[] = "Content-Length: ";
			const char HOST_TAG[] = "Host: ";
		}


		namespace JSON
		{
			namespace Key
			{
				const char QUERY_TYPE[] = "query type";

				const char CURTAIN[] = "curtain";
				const char CURTAIN_ID[] = "id";
				const char AUTO_CALIBRATE[] = "auto calibrate";
				const char AUTO_CORRECT[] = "auto correct";
				const char CURTAIN_POSITION[] = "position";
				const char DIRECTION[] = "direction";
				const char DISCRETE_MOVEMENT[] = "discrete movement";
				const char LENGTH[] = "length";

				const char EVENT[] = "event";
				const char EVENT_ID[] = "id";
				const char EVENT_FORCE[] = "FORCE";
				const char EVENT_POSITION[] = "desired position";
			}


			namespace Value
			{
				const char MOVE[] = "move";  // Query type value for move
				const char RESET[] = "reset";  // Query type value for reset
				const char STATUS[] = "status";  // Query type value for status

				const ValueID VALUE_IDS[] =
				{
					{MOVE_ID, MOVE},
					{RESET_ID, RESET},
					{STATUS_ID, STATUS}
				};
			}
		}


		namespace Responses
		{
			const uint8_t INVALID[] = "{\"error\" : \"Package received does not match JSON format\"}";
			const uint8_t VALID[] = "{\"success\":\"Valid JSON received\"}";
		}
	}


	// ——————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————— //

	// Get the id for a given value from the Literal::JSON::Value::VALUE_IDS.
	// Takes the value to match to.
	uint8_t id_for_value(const char* value)
	{
		for(uint8_t x = 0; x < sizeof(Literal::JSON::Value::VALUE_IDS) / sizeof(Literal::JSON::Value::ValueID); x++)
		{
			if(C_String::equal((char*)Literal::JSON::Value::VALUE_IDS[x].value, (char*)value))
			{
				return Literal::JSON::Value::VALUE_IDS[x].id;
			}
		}

		Exceptions::throw_generic("No matching value found");
	}

	
	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //

	char* http_exception_json(uint16_t error_code, char error_message[])
	{
		// TODO: Use JSON library
		// { "success" : false, "status code" : xxx, "message" : "" }
		uint16_t malloc_length = 60 + C_String::length(error_message);
		char* json_head = (char*)malloc(malloc_length), *json = json_head;

		C_String::copy(json, "{\"success\": false, \"status code\": ");
		json += C_String::length(json);
		C_String::itoa(error_code, json);

		C_String::copy(json+3, ", \"message\": \"");
		json += C_String::length(json+3) + 3;
		C_String::copy(json, error_message);
		json += C_String::length(json);

		C_String::copy_n(json, "\"}", 2);

		return json;
	}


	static char* status_json()
	{
		// TODO: Use JSON library
		// "{ "" : 1234567890 , "" : 1234567890 }\0" plus keys
		char* status_head = (char*)malloc(sizeof(Literal::JSON::Key::CURTAIN_ID)
		  +sizeof(Literal::JSON::Key::CURTAIN_POSITION)+38);
		C_String::copy_n("{\"", status_head, 2);
		char* status = status_head+2;

		C_String::copy(Literal::JSON::Key::CURTAIN_ID, status);
		status += sizeof(Literal::JSON::Key::CURTAIN_ID) - 1;  // Null terminator
		C_String::copy_n("\": ", status, 3);
		C_String::copy(Config::Curtain::CURTAIN_ID, status+3);
		status += C_String::length(status+3) + 3;
		C_String::copy_n(", \"", status, 3);

		C_String::copy(Literal::JSON::Key::CURTAIN_POSITION, status+3);
		status += sizeof(Literal::JSON::Key::CURTAIN_POSITION) + 2;
		C_String::copy_n("\": ", status, 3);
		C_String::itoa(Global::curtain.position(), status+3);
		status += C_String::length(status+3) + 3;

		C_String::copy_n("}", status, 2);

		return status_head;
	}


	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //

	// Skips the header read in from the client.
	// Reads through each character from client matching the character to the state machine to match to a double 
	//  carriage return-newline combo.
	// Return whether the body of the client is found.
	bool skip_header()
	{
		WiFiClient* client = &Global::client;  //SUGAR
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
	WiFiClient wait_for_request()
	{
		WiFiClient client = Global::server.available();
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

		if(!content || !skip_header()) return (char*)NULL;

		uint16_t x;
		for(x = 0; Global::client.available(); x++)
		{
			content[x] = Global::client.read();
		}
		content[x] = 0;
		if(JSON_BUFFER_SIZE < x)
		{
			delete content;
			return (char*)NULL;
		}

		return content;
	}


	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //

	// SUMMARY:	Writes the post request to the client adding headers to imply JSON.
	// PARAMS:	Takes the JSON string to write to send, the client's path to send to.
	// DETAILS:	
	// void post_json(char json[])
	void post_json(char json[], const uint8_t path[])
	{
		// Start line
		Global::client.print(Literal::HTTP::POST_METHOD);
		Global::client.print(String((const char*)path));
		Global::client.println(Literal::HTTP::HTTP_VERSION);

		// Headers
		Global::client.print(Literal::HTTP::HOST_TAG);
		Global::client.println(Config::Network::HUB_HOST_STR);

		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(C_String::length(json));

		// Contents
		Global::client.println();
		Global::client.println(json);
	}


	void respond_with_json_and_stop(char json[], const char response_type[])
	{
		// Start line
		Global::client.println(response_type);

		// Headers
		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(C_String::length(json));

		// Contents
		Global::client.println();
		Global::client.println(json);

		Global::client.stop();
	}


	void send_status_and_stop_client()
	{
		char* status_string = status_json();
		respond_with_json_and_stop(status_string);
		delete[] status_string;
	}


	// ————————————————————————————————————————— CONNECT TO & SEND HUB DATA ————————————————————————————————————————— //

	void update_hub(byte packet_buffer[])
	{
		if(Global::client.connected()) Global::client.stop();  // make sure I wasn't incompetent :)

		// Establish connection
		static WiFiClient client;
		Global::client = client;
		if(!Global::client.connect(Config::Network::HUB_HOST_STR, Config::Network::PORT)) return;

		// Send data if eventually connected
		uint8_t timeout;
		for(timeout = 255; !Global::client.connected() && timeout; timeout--) delay(10);
		if(timeout)
		{
			post_json((char*)packet_buffer, Config::Transmission::ACTION_COMPLETE_URL);
			while(Global::client.available()) Global::client.read();
			Global::client.stop();
		}
	}
}