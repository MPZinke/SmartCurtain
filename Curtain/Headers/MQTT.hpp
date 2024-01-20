
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


#include "Config.hpp"


#include "StaticString.hpp"


namespace DeserializedJSON
{
	class DeserializedJSON;
}


namespace MQTT
{
	namespace Topics
	{
		namespace Literals
		{
			const char ERROR[] = "SmartCurtain/error";  // Home Assistant subscribed topic to know Curtain's error.
			const char STATUS[] = "SmartCurtain/status";  // Home Assistant subscribed topic to know Curtain's status.

			const char HOME_MOVE[] = "SmartCurtain/Home/move";
		}

		namespace Parts
		{
			const char ROOM[] = "SmartCurtain/Room/";
			const char CURTAIN[] = "SmartCurtain/Curtain/";
			const char MOVE[] = "move";
			const char UPDATE[] = "update";
		}

		namespace Sizes
		{
			using namespace ::MQTT::Topics::Parts;

			const uint16_t ROOM_MOVE = sizeof(ROOM) + sizeof(Config::Curtain::ROOM) + sizeof(MOVE) - 2;
			const uint16_t CURTAIN_MOVE = sizeof(CURTAIN) + sizeof(Config::Curtain::NAME) + sizeof(MOVE) - 2;
			const uint16_t CURTAIN_UPDATE = sizeof(CURTAIN) + sizeof(Config::Curtain::NAME) + sizeof(UPDATE) - 2;
		}

		using namespace Parts;

		const StaticString<Sizes::ROOM_MOVE-1> ROOM_MOVE(ROOM, Config::Curtain::ROOM, MOVE);
		const StaticString<Sizes::CURTAIN_MOVE-1> CURTAIN_MOVE(CURTAIN, Config::Curtain::NAME, MOVE);
		const StaticString<Sizes::CURTAIN_UPDATE-1> CURTAIN_UPDATE(CURTAIN, Config::Curtain::NAME, UPDATE);
	}

	// ———————————————————————————————————————————————— RECEIVE DATA ———————————————————————————————————————————————— //
	DeserializedJSON::DeserializedJSON read_message(int message_size);

	// ————————————————————————————————————————————————— RESPONDING ————————————————————————————————————————————————— //
	void update_hub();
}  // end namespace Message
