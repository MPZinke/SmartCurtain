
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


#include <ArduinoJson.h>
#include <WiFiClient.h>


#include "Config.hpp"


namespace Message
{
	namespace Literal
	{
		namespace HTTP
		{
			// ———— START LINE ———— //
			extern const char OK_REQUEST[];  // start string for valid request from device
			extern const char NO_CONTENT_REQUEST[];  // start string for no content for request
			extern const char BAD_REQUEST[];  // start string for invalid request from device
			extern const char UNAUTHORIZED[];  // start string for invalid request from device
			extern const char FORBIDDEN[];  // start string for invalid request from device
			extern const char NOT_FOUND_REQUEST[];  // start string for no content for request
			extern const char INTERNAL_SERVER_ERROR_REQUEST[];
			// —— START LINE::POST —— //
			extern const char PATCH_METHOD[];
			extern const char POST_METHOD[];
			extern const char HTTP_VERSION[];

			// ———— HEADERS ———— //
			extern const char AUTHORIZATION_HEADER[];
			extern const char CONTENT_TYPE[];
			extern const char CONTENT_LENGTH_TAG[];
			extern const char HOST_TAG[];

			// ———— OTHER ———— //
			extern const uint32_t AUTH_TAG_SIZE;
		}  // end namespace HTTP


		namespace JSON
		{
			namespace Key
			{
				extern const char QUERY_TYPE[];

				extern const char CURTAIN[];
				extern const char CURTAIN_ID[];
				extern const char AUTO_CALIBRATE[];
				extern const char AUTO_CORRECT[];
				extern const char CURTAIN_PERCENTAGE[];
				extern const char CURTAIN_POSITION[];
				extern const char DIRECTION[];
				extern const char DISCRETE_MOVEMENT[];
				extern const char LENGTH[];

				extern const char EVENT[];
				extern const char EVENT_ID[];
				extern const char EVENT_FORCE[];
				extern const char EVENT_IS_FINISHED[];
				extern const char EVENT_PERCENTAGE[];

				extern const char HUB_IP[];
			}  // end namespace Key


			namespace Value
			{
				extern const char MOVE[];  // Query type value for move
				extern const char RESET[];  // Query type value for reset
				extern const char STATUS[];  // Query type value for status
				extern const char UPDATE[];  // Query type value for updating curtain details

				// Used to auto increment the ID for the value
				enum
				{
					UNDEFINED = 0,
					MOVE_ID,
					RESET_ID,
					STATUS_ID,
					UPDATE_ID
				};


				typedef struct
				{
					const uint8_t id;
					const char* value;
				} ValueID;


				extern const ValueID VALUE_IDS[];
			}  // end namespace Value
		}  // end namespace JSON


		namespace Responses
		{
			extern const char INVALID[];
			extern const char MOVING[];
			extern const char VALID[];
		}  // end namespace Responses
	}  // end namespace Literal


	// ———— UTILITY ———— //
	String convert_JsonObject_to_String(JsonObject& object);
	void deactivate_curtain();
	uint8_t id_for_query_type(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document);
	// ———— JSON PRODUCERS ———— //
	String http_exception_json(uint16_t error_code, char error_message[]);
	String status_json();
	// ———— RECEIVE DATA ———— //
	StaticJsonDocument<JSON_BUFFER_SIZE> decode_json();
	String read_request_data_into_buffer();
	void read_to_next_line();
	bool skip_header();
	bool unauthenticated();
	bool unauthorized();
	WiFiClient wait_for_request();
	// ———— RESPONDING ———— //
	void respond_with_json_and_stop(String& json, const char response_type[]=Literal::HTTP::OK_REQUEST);
	void respond_with_json_and_stop(const char json[], const char response_type[]=Literal::HTTP::OK_REQUEST);
	void write_json(char json[], const char path[], const char method[]=Literal::HTTP::POST_METHOD);
  	// ———— CONNECT CONNECTION ———— //
	void clear_buffer_and_stop_client();
	bool new_global_client_connection();
} // end namespace Message
