
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


#include "../Headers/Message.hpp"

#include "../Headers/Config.hpp"
#include "../Headers/Global.hpp"

#include "../Headers/C_String.hpp"
#include "../Headers/Curtain.hpp"
#include "../Headers/Event.hpp"
#include "../Headers/Exceptions.hpp"


using namespace Exceptions;


namespace Message
{
	namespace Literal
	{
		namespace HTTP
		{
			// ———— START LINE ———— //
			const char OK_REQUEST[] = "HTTP/1.1 200 OK";  // start string for valid request from device
			const char NO_CONTENT_REQUEST[] = "HTTP/1.1 204 No Content";  // start string for no content for request
			const char BAD_REQUEST[] = "HTTP/1.1 400 Bad Message";  // start string for invalid request from device
			const char UNAUTHORIZED[] = "HTTP/1.1 401 UNAUTHORIZED";  // start string for invalid request from device
			const char FORBIDDEN[] = "HTTP/1.1 401 FORBIDDEN";  // start string for invalid request from device
			const char NOT_FOUND_REQUEST[] = "HTTP/1.1 404 Not Found";  // start string for no content for request
			const char INTERNAL_SERVER_ERROR_REQUEST[] = "HTTP/1.1 500 Internal Server Error";
			// —— START LINE::POST —— //
			const char PATCH_METHOD[] = "PATCH ";
			const char POST_METHOD[] = "POST ";
			const char HTTP_VERSION[] = " HTTP/1.1";

			// ———— HEADERS ———— //
			const char AUTHORIZATION_HEADER[] = "Authorization: ";
			const char CONTENT_TYPE[] = "Content-Type: application/json";
			const char CONTENT_LENGTH_TAG[] = "Content-Length: ";
			const char HOST_TAG[] = "Host: ";

			// ———— OTHER ———— //
			// minus 1 for \0
			const uint32_t AUTH_HEADER_LENGTH = (sizeof(Literal::HTTP::AUTHORIZATION_HEADER) / sizeof(char) - 1);
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
				const char CURTAIN_PERCENTAGE[] = "percentage";
				const char CURTAIN_POSITION[] = "position";
				const char DIRECTION[] = "direction";
				const char DISCRETE_MOVEMENT[] = "discrete movement";
				const char LENGTH[] = "length";

				const char EVENT[] = "event";
				const char EVENT_ID[] = "id";
				const char EVENT_FORCE[] = "FORCE";
				const char EVENT_IS_FINISHED[] = "is finished";
				const char EVENT_PERCENTAGE[] = "percentage";

				const char HUB_IP[] = "Hub IP";
			}


			namespace Value
			{
				const char MOVE[] = "move";  // Query type value for move
				const char RESET[] = "reset";  // Query type value for reset
				const char STATUS[] = "status";  // Query type value for status
				const char UPDATE[] = "update";  // Query type value for update

				const ValueID VALUE_IDS[] =
				{
					{MOVE_ID, MOVE},
					{RESET_ID, RESET},
					{STATUS_ID, STATUS},
					{UPDATE_ID, UPDATE}
				};
			}
		}


		namespace Responses
		{
			const char INVALID[] = "{\"error\" : \"Package received does not match JSON format\"}";
			const char MOVING[] = "{\"success\":\"Moving to position\"}";
			const char VALID[] = "{\"success\":\"Valid JSON received\"}";
		}
	}


	// ——————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————— //

	String convert_JsonObject_to_String(JsonObject& object)
	{
		String json_string;
		serializeJson(object, json_string);

		return json_string;
	}


	void deactivate_curtain()
	{
		if(new_global_client_connection())
		{
			String path = String("/api/curtains/") + Global::curtain.id() + "/is_activated/" + false;
			String curtain_json = Global::curtain;
			write_json((char*)curtain_json.c_str(), path.c_str(), Literal::HTTP::PATCH_METHOD);
			clear_buffer();
			Global::client.stop();
		}
	}


	// Get the id for a given value from the Literal::JSON::Value::VALUE_IDS.
	// Takes the value to match to.
	uint8_t id_for_query_type(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		if(!json_document.containsKey(Literal::JSON::Key::QUERY_TYPE))
		{
			String message = String("JSON is mssing values '") + Literal::JSON::Key::QUERY_TYPE + "'";
			new NOT_FOUND_404_Exception(__LINE__-2, __FILE__, message);
			return Literal::JSON::Value::UNDEFINED;
		}

		const char* value = json_document[Literal::JSON::Key::QUERY_TYPE];
		for(uint8_t x = 0; x < sizeof(Literal::JSON::Value::VALUE_IDS) / sizeof(Literal::JSON::Value::ValueID); x++)
		{
			if(C_String::equal(value, Literal::JSON::Value::VALUE_IDS[x].value))
			{
				return Literal::JSON::Value::VALUE_IDS[x].id;
			}
		}

		return Literal::JSON::Value::UNDEFINED;
	}

	
	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //

	String http_exception_json(uint16_t error_code, char error_message[])
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject error_object = json_document.to<JsonObject>();

		error_object["success"] = false;
		error_object["status code"] = error_code;
		error_object["message"] = error_message;

		return convert_JsonObject_to_String(error_object);
	}


	String status_json()
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject status_object = json_document.to<JsonObject>();

		Global::curtain.append_to(status_object);
		Global::event.append_to(status_object);

		return convert_JsonObject_to_String(status_object);
	}


	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //
	
	void clear_buffer(uint32_t offset/*=0*/)
	{
		for(uint32_t x = offset; x < 0xFFFFFFFF && Global::client.available(); x++)
		{
			Global::client.read();
		}
	}


	StaticJsonDocument<JSON_BUFFER_SIZE> read_message()
	{
		Global::client = wait_for_request();

		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		uint32_t read_count = 0;

		read_headers(read_count);
		if(Global::exception)
		{
			return json_document;
		}

		String json_buffer = read_body(read_count);
		// ≈ if(Global::exception) return json; else if(deserializeJson(...)) new BAD_REQUEST_400_Exception(...);
		if(!Global::exception && deserializeJson(json_document, json_buffer))
		{
			new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Could not read request into json_buffer");
		}

		return json_document;
	}


	void read_headers(register uint32_t& read_count)
	{
		if(unauthenticated(read_count))
		{
			new UNAUTHORIZED_401_Exception(__LINE__, __FILE__, "No Authorization header found");
		}
		else if(unauthorized(read_count))
		{
			new FORBIDDEN_403_Exception(__LINE__, __FILE__, "Not Authorized");
		}

		if(!skip_header(read_count))
		{
			new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Could not read request into json_buffer");
		}
	}

	/*
	SUMMARY: Reads the client request into a dynamically allocated buffer.
	DETAILS: Skips over the content headers. Allocates enough memory for content length and reads it in from client
	         into buffer.
	RETURNS: Populated buffer if successfully read, otherwise NULL pointer to indicate error occuring.
	*/
	String read_body(register uint32_t& read_count)
	{
		String content;
		for(uint16_t x = 0; Global::client.available() && x < JSON_BUFFER_SIZE; x++, read_count++)
		{
			content += (char)Global::client.read();
		}

		if(JSON_BUFFER_SIZE > content.length())
		{
			return content;
		}

		// If client contents longer than content, handle "error" and do not proceed
		new BAD_REQUEST_400_Exception(__LINE__, __FILE__, "Data length is too large for JSON Buffer");
		clear_buffer(read_count);
		return String();
	}


	void read_to_next_line(register uint32_t& read_count)
	{
		for(; read_count < 0xFFFFFFFF && Global::client.available() >= 2; read_count++)
		{
			if(Global::client.read() == '\r' && Global::client.read() == '\n')
			{
				return;
			}
		}
	}


	/*
	SUMMARY: Skips the header read in from the client.
	DETAILS: Reads through each character from client matching the character to the state machine to match to a double 
	         carriage return-newline combo.
	RETURNS: Return whether the body of the client is found.
	*/
	bool skip_header(register uint32_t& read_count)
	{
		for(; read_count < 0xFFFFFFFF && Global::client.available() >= 4; read_count++)
		{
			// State machine
			if(Global::client.read() == '\r' && Global::client.read() == '\n'
			  && Global::client.read() == '\r' && Global::client.read() == '\n')
			{
				return true;
			}
		}
		return false;
	}


	bool unauthenticated(register uint32_t& read_count)
	{
		// Match "Authorization: " tag
		for(register uint32_t header_index = 0; read_count < 0xFFFFFFFF && Global::client.available(); read_count++)
		{
			char next = Global::client.read();
			// If starting a newline and another is found, data section reached; no auth header has been found
			if(next == '\r' && Global::client.available() && (next = Global::client.read()) == '\n')
			{
				return true;
			}

			// If the tag does not match...
			if(next != Literal::HTTP::AUTHORIZATION_HEADER[header_index])
			{
				read_to_next_line(read_count);
				header_index = 0;
			}
			// Tag matches Auth tag
			else if(header_index == Literal::HTTP::AUTH_HEADER_LENGTH-1)  // minus 1 for indexing
			{
				return false;
			}
			// Keep trying
			else
			{
				header_index++;
			}
		}

		return true;  // should never happen
	}


	/*
	SUMMARY: Determines whether the auth value is correct.
	DETAILS: Reads through each character from client matching the character to the state machine to match to a double 
	         carriage return-newline combo.
	RETURNS: Return whether the body of the client is found.
	*/
	bool unauthorized(register uint32_t& read_count)
	{
		for(register uint32_t x = 0; x < Config::AUTH_VALUE_LENGTH && Global::client.available(); x++, read_count++)
		{
			if(Global::client.read() != Config::AUTHORIZATION_VALUE[x])
			{
				return true;
			}
		}

		return Global::client.available() && Global::client.peek() == '\r';
	}


	/*
	SUMMARY: Waits until client request received by the server, then converts and returns an HttpClient.
	DETAILS: Creates static variable to stay alive for reference by returned HttpClient. Stays in loop until a client
	         is successfully returned by the server. Converts client to HttpClient.
	RETURNS: HttpClient referncing client.
	*/
	WiFiClient wait_for_request()
	{
		WiFiClient client = Global::server.available();
		while(!client)
		{
			delayMicroseconds(10);
			client = Global::server.available();
		}

		Global::client_IP = client.remoteIP();
		return client;
	}


	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //

	void respond_with_json_and_stop(String& json, const char response_type[]/*=Literal::HTTP::OK_REQUEST*/)
	{
		// Start line
		Global::client.println(response_type);

		// Headers
		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::AUTHORIZATION_HEADER);
		Global::client.println(Config::HUB_AUTHORIZATION_VALUE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(json.length());

		// Contents
		Global::client.println();
		Global::client.println(json);

		Global::client.stop();
	}


	void respond_with_json_and_stop(const char json[], const char response_type[]/*=Literal::HTTP::OK_REQUEST*/)
	{
		// Start line
		Global::client.println(response_type);

		// Headers
		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::AUTHORIZATION_HEADER);
		Global::client.println(Config::HUB_AUTHORIZATION_VALUE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(C_String::length((char*)json));

		// Contents
		Global::client.println();
		Global::client.println(json);

		Global::client.stop();
	}


	/*
	SUMMARY: Writes the post request to the client adding headers to imply JSON.
	PARAMS:	 Takes the JSON string to write to send, the client's path to send to.
	DETAILS: 
	*/
	void write_json(char json[], const char path[], const char method[]/*=Literal::HTTP::POST_METHOD*/)
	{
		// Start line
		Global::client.print(method);
		Global::client.print(String((const char*)path));
		Global::client.println(Literal::HTTP::HTTP_VERSION);

		// Headers
		Global::client.print(Literal::HTTP::HOST_TAG);
		Global::client.println(Global::client_IP);

		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::AUTHORIZATION_HEADER);
		Global::client.println(Config::HUB_AUTHORIZATION_VALUE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(C_String::length(json));

		// Contents
		Global::client.println();
		Global::client.println(json);
	}


	// ————————————————————————————————————————————— CONNECT CONNECTION ————————————————————————————————————————————— //

	bool new_global_client_connection()
	{
		if(Global::client.connected())
		{
			clear_buffer();
			Global::client.stop();
		}

		// Establish connection
		// static WiFiClient client;
		Global::client = WiFiClient();

		if(!Global::client.connect(Global::client_IP, Config::Network::PORT))
		{
			return false;
		}

		// Send data if eventually connected
		uint8_t timeout;
		for(timeout = 255; !Global::client.connected() && timeout; timeout--)
		{
			delay(10);
		}

		return Global::client.connected();
	}
}