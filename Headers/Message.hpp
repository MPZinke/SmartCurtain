
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


namespace DeserializedJSON
{
	class DeserializedJSON;
}


namespace Message
{
	namespace Literal
	{
		namespace MQTT
		{
			extern const char PATH_PREFIX[];
			extern const char CURTAIN_PATH_PREFIX[];
			extern const char ALL_CURTAINS[];
			extern const char MOVE_SUFFIX[];
			extern const char STATUS_SUFFIX[];
			extern const char UPDATE_SUFFIX[];
			extern const char HUB_UPDATE_TOPIC[];
			extern const char HUB_ERROR_TOPIC[];
		}  // end namespace MQTT


		namespace JSON
		{
			namespace Key
			{
				// CURTAIN
				// Structure
				extern const char CURTAIN_ID[];
				extern const char HOME_ID[];
				extern const char ROOM_ID[];

				// Hardware describing/overriding values
				extern const char LENGTH[];
				extern const char PERCENTAGE[];

				// Movement describing/overriding values
				extern const char AUTO_CORRECT[];

				// Non-Overridable
				extern const char IS_MOVING[];
			}  // end namespace Key
		}  // end namespace JSON
	}  // end namespace Literal


	// —————————————————————————————————————————————————— UTILITY  —————————————————————————————————————————————————— //
	// ——————————————————————————————————————————————— JSON PRODUCERS ——————————————————————————————————————————————— //
	String convert_JsonObject_to_String(JsonObject& object);
	String http_exception_json(uint16_t error_code, char error_message[]);
	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //
	DeserializedJSON::DeserializedJSON read_message(int message_size);
	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //
	void update_hub();
} // end namespace Message
