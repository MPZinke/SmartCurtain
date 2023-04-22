
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
		namespace MQTT
		{
			extern const char CURTAIN_PATH_PREFIX[];
			extern const char MOVE_SUFFIX[];
			extern const char STATUS_SUFFIX[];
			extern const char UPDATE_SUFFIX[];
			extern const char HUB_UPDATE_TOPIC[];
		}  // end namespace MQTT


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

				extern const char EVENT_IS_MOVING[];
				extern const char EVENT_PERCENTAGE[];

				extern const char HUB_IP[];
			}  // end namespace Key
		}  // end namespace JSON
	}  // end namespace Literal


	// —————————————————————————————————————————————————— UTILITY  —————————————————————————————————————————————————— //
	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //
	inline String convert_JsonObject_to_String(JsonObject& object);
	String http_exception_json(uint16_t error_code, char error_message[]);
	String status_json();
	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //
	DeserializedJSON read_message(int message_size);
	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //
	void update_hub();
} // end namespace Message
