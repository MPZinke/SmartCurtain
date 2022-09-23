
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.12.19                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "../Headers/Config.hpp"


namespace Config
{
	const char AUTHORIZATION_VALUE[] = "HelloWorld";
	const uint32_t AUTH_VALUE_LENGTH = sizeof(AUTHORIZATION_VALUE) - 1;

	namespace Curtain
	{
		const char CURTAIN_ID[] = "1";

		const uint32_t IGNORE_MOVEMENT_SIMILARITY = 10;  // max step difference to ignore event
		const uint32_t POSITION_TOLLERANCE = 5;  // steps within ends to consider "close enough"
		const uint32_t STEPS_FOR_CALIBRATION = 5;  // how picky the program should be to commence recalibration
	}


	namespace Hardware
	{
		// ———————— HARDWARE DICTATING VALUES ————————
		// ———— PINS ————
		const uint8_t DIRECTION_PIN = 5;
		const uint8_t ENABLE_PIN = 18;
		const uint8_t PULSE_PIN = 19;

		const uint8_t CLOSE_PIN = 21;
		const uint8_t OPEN_PIN = 0;

		// ———— WAIT ————
		const uint16_t PULSE_WAIT = 60;  // Wait between pulses for the stepper motor

		// ———————— HARDWARE DETERMINED VALUES ————————
		const bool CLOSE_ENDSTOP = (bool)CLOSE_PIN;
		const bool OPEN_ENDSTOP = (bool)OPEN_PIN;
		const bool BOTH_ENDSTOPS = CLOSE_ENDSTOP && OPEN_ENDSTOP;
		const bool EITHER_ENDSTOP = CLOSE_ENDSTOP || OPEN_ENDSTOP;

		// These are determined by the hardware, as opposed to the hardware functionality being determined by these.
		// ———— STEPS ————
		const uint32_t DEFAULT_LENGTH = 34000;

		// ———— SWITCHES ————
		// I don't like discrete movement, so I am not going to allow it on my curtain.
		const bool DISCRETE_MOVEMENT_ALLOWED = false;
		const bool DIRECTION_SWITCH = false;  // Used to make OPEN == ON and CLOSE == OFF.
		const bool SWITCH = true;  // true = LOW is ON or false = HIGH is ON (depends on electronic current directions)
	}


	namespace Network
	{
		// NODE
		const uint16_t PORT = 80;  // listening port
		uint8_t MAC_ADDRESS[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}

		// ROUTER
		uint8_t ROUTER_GATEWAY[] = {10,0,0,1};  // router gateway
		uint8_t SUBNET_MASK[] = {255,255,255,0};  // of the router
		const char PASSWORD[] = "";  // WiFi password
		const char SSID[] = "";  // WiFi name
	}  // end namespace Config::Network
}  // end namespace Configure