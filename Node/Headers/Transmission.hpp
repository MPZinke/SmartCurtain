
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


#include "Global.hpp"

#include "C_String.hpp"
#include "Exceptions.hpp"


namespace Transmission
{
	namespace Literal
	{
		namespace HTTP
		{
			// ———— START LINE ———— //
			extern const char VALID_REQUEST[];  // start string for valid request from device
			extern const char INVALID_REQUEST[];  // start string for invalid request from device
			extern const char NO_CONTENT_REQUEST[];  // start string for no content for request
			extern const char NOT_FOUND_REQUEST[];  // start string for no content for request
			// —— START LINE::POST —— //
			extern const char POST_METHOD[];
			extern const char HTTP_VERSION[];

			// ———— HEADERS ———— //
			extern const char CONTENT_TYPE[];
			extern const char CONTENT_LENGTH_TAG[];
			extern const char HOST_TAG[];
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
			extern const uint8_t INVALID[];
			extern const uint8_t VALID[];
		}  // end namespace Responses
	}  // end namespace Literal


	char* convert(JsonObject& object);
	uint8_t id_for_value(const char* value);
	char* http_exception_json(uint16_t error_code, char error_message[]);
	static char* status_json();
	bool skip_header();
	WiFiClient wait_for_request();
	char* read_transmission_data_into_buffer();
	void post_json(char json[], const uint8_t path[]=Config::Transmission::ACTION_COMPLETE_URL);
	void respond_with_json_and_stop(char json[], const char response_type[]=Literal::HTTP::VALID_REQUEST);
	void send_status_and_stop_client();
	void update_hub(byte packet_buffer[]);
} // end namespace Transmission
