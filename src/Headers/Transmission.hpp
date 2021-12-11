
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


#include "Global.hpp"


namespace Transmission
{
	namespace Literals
	{
		namespace HTTP
		{
			// ———— START LINE ———— //
			const char VALID_REQUEST_STR[] = "HTTP/1.1 200 OK";  // start string for valid request from device
			const char INVALID_REQUEST_STR[] = "HTTP/1.1 400 Bad Request";  // start string for invalid request from device
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
				const char CURRENT_POS[] = "current position";
				const char LENGTH[] = "length";
				const char CALIBRATE[] = "auto calibrate";
				const char CORRECT[] = "auto correct";
				const char IS_SMART[] = "is smart";

				const char EVENT[] = "event";
				const char EVENT_ID[] = "id";
				const char EVENT_DESIRED_POS[] = "desired position";
			}


			namespace Value
			{
				constexpr char MOVE[] = "move";  // Query type value for move
				constexpr char RESET[] = "reset";  // Query type value for reset
				constexpr char STATUS[] = "status";  // Query type value for status
			}
		}


		namespace Responses
		{
			const uint8_t INVALID_RESPONSE[] = "{\"error\" : \"Package received does not match JSON format\"}";
			const uint8_t VALID_RESPONSE[] = "{\"success\":\"Valid JSON received\"}";
		}
	}

	char* http_exception_json(char error_message[]);
	static char* status_json();
	bool skip_header();
	WiFiClient wait_for_request();
	char* read_transmission_data_into_buffer();
	void post_json(char json[], const uint8_t path[]=Config::Transmission::ACTION_COMPLETE_URL);
	void respond_with_json_and_stop(char json[], const char response_type[]=Literals::HTTP::VALID_REQUEST_STR);
	void send_status_and_stop_client();
	void update_hub(byte packet_buffer[]);

} // end namespace Transmission
