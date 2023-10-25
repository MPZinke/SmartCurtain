
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


#include <ArduinoJson.h>


#include "StaticString.hpp"


#define HOME_PREFIX "SmartCurtain/000000000000000000000000"
#define ROOM_PREFIX "SmartCurtain/-/000000000000000000000000"
#define CURTAIN_PREFIX "SmartCurtain/-/-/000000000000000000000000"

#define MOVE_SUFFIX "/move"
#define STATUS_SUFFIX "/status"
#define UPDATE_SUFFIX "/update"


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
		// 	extern const char HOME_MOVE[/* 42+1 */];
		// 	extern const char HOME_UPDATE[/* 44+1 */];

		// 	extern const char ROOM_MOVE[/* 44+1 */];
		// 	extern const char ROOM_UPDATE[/* 46+1 */];

		// 	extern const char ALL_CURTAINS_MOVE[];
		// 	extern const char ALL_CURTAINS_STATUS[];
		// 	extern const char CURTAIN_MOVE[/* 46+1 */];
		// 	extern const char CURTAIN_STATUS[/* 48+1 */];
		// 	extern const char CURTAIN_UPDATE[/* 48+1 */];
			extern StaticString<sizeof(HOME_PREFIX)+sizeof(MOVE_SUFFIX)-1> HOME_MOVE;
			extern StaticString<sizeof(HOME_PREFIX)+sizeof(UPDATE_SUFFIX)-1> HOME_UPDATE;

			extern StaticString<sizeof(ROOM_PREFIX)+sizeof(MOVE_SUFFIX)-1> ROOM_MOVE;
			extern StaticString<sizeof(ROOM_PREFIX)+sizeof(UPDATE_SUFFIX)-1> ROOM_UPDATE;

			extern const char ALL_CURTAINS_MOVE[];
			extern const char ALL_CURTAINS_STATUS[];

			extern StaticString<sizeof(CURTAIN_PREFIX)+sizeof(MOVE_SUFFIX)-1> CURTAIN_MOVE;
			extern StaticString<sizeof(CURTAIN_PREFIX)+sizeof(STATUS_SUFFIX)-1> CURTAIN_STATUS;
			extern StaticString<sizeof(CURTAIN_PREFIX)+sizeof(UPDATE_SUFFIX)-1> CURTAIN_UPDATE;
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
	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //
	DeserializedJSON::DeserializedJSON read_message(int message_size);
	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //
	void update_hub();
} // end namespace Message
