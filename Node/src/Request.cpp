
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


#include "../Headers/Request.hpp"

#include "../Headers/C_String.hpp"
#include "../Headers/Curtain.hpp"
#include "../Headers/Exceptions.hpp"


namespace Request
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
			const char PATCH_METHOD[] = "PATCH ";
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
				const char CURTAIN_PERCENTAGE[] = "percentage";
				const char CURTAIN_POSITION[] = "position";
				const char DIRECTION[] = "direction";
				const char DISCRETE_MOVEMENT[] = "discrete movement";
				const char LENGTH[] = "length";

				const char EVENT[] = "event";
				const char EVENT_ID[] = "id";
				const char EVENT_FORCE[] = "FORCE";
				const char EVENT_PERCENTAGE[] = "desired position";

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
			const char VALID[] = "{\"success\":\"Valid JSON received\"}";
		}
	}


	// ——————————————————————————————————————————————— UTILITY ——————————————————————————————————————————————— //

	String convert(JsonObject& object)
	{
		String json_string;
		serializeJson(object, json_string);

		return json_string;
	}


	// Get the id for a given value from the Literal::JSON::Value::VALUE_IDS.
	// Takes the value to match to.
	uint8_t id_for_value(const char* value)
	{
		for(uint8_t x = 0; x < sizeof(Literal::JSON::Value::VALUE_IDS) / sizeof(Literal::JSON::Value::ValueID); x++)
		{
			if(C_String::equal(value, Literal::JSON::Value::VALUE_IDS[x].value))
			{
				return Literal::JSON::Value::VALUE_IDS[x].id;
			}
		}

		Exceptions::throw_generic("No matching value found");
	}

	
	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //

	String http_exception_json(uint16_t error_code, char error_message[])
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject error_object = json_document.to<JsonObject>();

		error_object["success"] = false;
		error_object["status code"] = error_code;
		error_object["message"] = error_message;

		return convert(error_object);
	}


	String status_json()
	{
		StaticJsonDocument<JSON_BUFFER_SIZE> json_document;
		JsonObject status_object = json_document.to<JsonObject>();

		status_object[Literal::JSON::Key::CURTAIN_ID] = Config::Curtain::CURTAIN_ID;
		status_object[Literal::JSON::Key::CURTAIN_POSITION] = Global::curtain.percentage();

		return convert(status_object);
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

		Global::client_IP = client.remoteIP();
		return client;
	}


	// SUMMARY:	Reads the client request into a dynamically allocated buffer.
	// DETAILS:	Skips over the content headers. Allocates enough memory for content length and reads it in from client
	//  into buffer.
	// RETURN:	Populated buffer if successfully read, otherwise NULL pointer to indicate error occuring.
	String read_request_data_into_buffer()
	{
		String content;
		if(!skip_header()) return content;

		for(uint16_t x = 0; Global::client.available() && x < JSON_BUFFER_SIZE; x++)
		{
			content += (char)Global::client.read();
		}

		// If client contents longer than content, handle "error" and do not proceed
		if(JSON_BUFFER_SIZE <= content.length())
		{
			for(uint32_t x = 0; x < 0xFFFFFFFF && Global::client.available(); x++)
			{
				Global::client.read();
			}
			content = "";
		}

		return content;
	}


	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //

	// SUMMARY:	Writes the post request to the client adding headers to imply JSON.
	// PARAMS:	Takes the JSON string to write to send, the client's path to send to.
	// DETAILS:	
	// void post_json(char json[])
	void write_json(char json[], const char path[]/*=Config::Transmission::ACTION_COMPLETE_URL*/,
	  const char method[]/*=Literal::HTTP::POST_METHOD*/)
	{
		// Start line
		Global::client.print(method);
		Global::client.print(String((const char*)path));
		Global::client.println(Literal::HTTP::HTTP_VERSION);

		// Headers
		Global::client.print(Literal::HTTP::HOST_TAG);
		Global::client.println(Global::client_IP);

		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(C_String::length(json));

		// Contents
		Global::client.println();
		Global::client.println(json);
	}


	void respond_with_json_and_stop(String& json, const char response_type[]/*=Literal::HTTP::VALID_REQUEST*/)
	{
		// Start line
		Global::client.println(response_type);

		// Headers
		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(json.length());

		// Contents
		Global::client.println();
		Global::client.println(json);

		Global::client.stop();
	}


	void respond_with_json_and_stop(const char json[], const char response_type[]/*=Literal::HTTP::VALID_REQUEST*/)
	{
		// Start line
		Global::client.println(response_type);

		// Headers
		Global::client.println(Literal::HTTP::CONTENT_TYPE);

		Global::client.print(Literal::HTTP::CONTENT_LENGTH_TAG);
		Global::client.println(C_String::length((char*)json));

		// Contents
		Global::client.println();
		Global::client.println(json);

		Global::client.stop();
	}


	void send_status_and_stop_client()
	{
		String status_string = status_json();
		respond_with_json_and_stop(status_string);
	}


	// ————————————————————————————————————————— CONNECT TO & SEND HUB DATA ————————————————————————————————————————— //

	void update_hub()
	{
		if(Global::client.connected()) Global::client.stop();  // make sure I wasn't incompetent :)

		// Establish connection
		static WiFiClient client;
		Global::client = client;
		if(!Global::client.connect(Global::client_IP, Config::Network::PORT)) return;

		// Send data if eventually connected
		uint8_t timeout;
		for(timeout = 255; !Global::client.connected() && timeout; timeout--) delay(10);
		if(timeout)
		{
			String path = String("/api/curtains/") + Global::curtain.id() + "/is_activated/" + false;
			String curtain_json = Global::curtain;
			write_json((char*)curtain_json.c_str(), path.c_str(), Literal::HTTP::PATCH_METHOD);
			while(Global::client.available()) Global::client.read();
			Global::client.stop();
		}
	}
}