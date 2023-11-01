
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


#define HOME_PREFIX "SmartCurtain/"
#define ROOM_PREFIX "SmartCurtain/-/"
#define CURTAIN_PREFIX "SmartCurtain/-/-/"

#define BLANK_OBJECT_ID "000000000000000000000000"

#define MOVE_SUFFIX "/move"
#define STATUS_SUFFIX "/status"
#define UPDATE_SUFFIX "/update"


#define HOME_MOVE_STRING HOME_PREFIX BLANK_OBJECT_ID MOVE_SUFFIX
#define HOME_UPDATE_STRING HOME_PREFIX BLANK_OBJECT_ID UPDATE_SUFFIX

#define ROOM_MOVE_STRING ROOM_PREFIX BLANK_OBJECT_ID MOVE_SUFFIX
#define ROOM_UPDATE_STRING ROOM_PREFIX BLANK_OBJECT_ID UPDATE_SUFFIX

#define CURTAIN_MOVE_STRING CURTAIN_PREFIX BLANK_OBJECT_ID MOVE_SUFFIX
#define CURTAIN_STATUS_STRING CURTAIN_PREFIX BLANK_OBJECT_ID STATUS_SUFFIX
#define CURTAIN_UPDATE_STRING CURTAIN_PREFIX BLANK_OBJECT_ID UPDATE_SUFFIX


namespace DeserializedJSON
{
	class DeserializedJSON;
}


namespace MQTT
{
	extern StaticString<sizeof(HOME_MOVE_STRING)> HOME_MOVE;
	extern StaticString<sizeof(HOME_UPDATE_STRING)> HOME_UPDATE;

	extern StaticString<sizeof(ROOM_MOVE_STRING)> ROOM_MOVE;
	extern StaticString<sizeof(ROOM_UPDATE_STRING)> ROOM_UPDATE;

	extern const char ALL_CURTAINS_MOVE[];
	extern const char ALL_CURTAINS_STATUS[];

	extern StaticString<sizeof(CURTAIN_MOVE_STRING)> CURTAIN_MOVE;
	extern StaticString<sizeof(CURTAIN_STATUS_STRING)> CURTAIN_STATUS;
	extern StaticString<sizeof(CURTAIN_UPDATE_STRING)> CURTAIN_UPDATE;

	extern const char HUB_UPDATE_TOPIC[];
	extern const char HUB_ERROR_TOPIC[];

	// —————————————————————————————————————————————————— UTILITY  —————————————————————————————————————————————————— //
	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //
	DeserializedJSON::DeserializedJSON read_message(int message_size);
	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //
	void update_hub();
} // end namespace Message
