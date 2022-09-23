
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2021.11.27                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include <stdint.h>


#define JSON_BUFFER_SIZE 0x400


namespace Config
{
	extern const char AUTHORIZATION_VALUE[];
	extern const char HUB_AUTHORIZATION_VALUE[];
	extern const uint32_t AUTH_VALUE_LENGTH;

	namespace Curtain
	{
		extern const char CURTAIN_ID[];

		extern const uint32_t IGNORE_MOVEMENT_SIMILARITY;  // max step difference to ignore event
		extern const uint32_t POSITION_TOLLERANCE;  // steps within ends to consider "close enough"
		extern const uint32_t STEPS_FOR_CALIBRATION;  // how picky the program should be to commence recalibration
	}


	namespace Hardware
	{
		// ———————— HARDWARE DICTATING VALUES ————————
		// ———— PINS ————
		extern const uint8_t DIRECTION_PIN;
		extern const uint8_t ENABLE_PIN;
		extern const uint8_t PULSE_PIN;

		extern const uint8_t CLOSE_PIN;
		extern const uint8_t OPEN_PIN;

		// ———— WAIT ————
		extern const uint16_t PULSE_WAIT;  // Wait between pulses for the stepper motor

		// ———————— HARDWARE DETERMINED VALUES ————————
		extern const bool CLOSE_ENDSTOP;
		extern const bool OPEN_ENDSTOP;
		extern const bool BOTH_ENDSTOPS;  // whether curtain contains both endstops
		extern const bool EITHER_ENDSTOP;

		// These are determined by the hardware, as opposed to the hardware functionality being determined by these.
		// ———— STEPS ————
		extern const uint32_t DEFAULT_LENGTH;

		// ———— SWITCHES ————
		extern const bool DIRECTION_SWITCH;  // Used to make OPEN == 100 and CLOSE == 0.
		extern const bool DISCRETE_MOVEMENT_ALLOWED;
		extern const bool SWITCH;  // true = LOW is ON or false = HIGH is ON (depends on electronic current directions)
	}


	namespace Network
	{
		// NODE
		extern const uint16_t PORT;  // listening port
		extern uint8_t MAC_ADDRESS[];  // {'Z', 'C', 'R', 'T', 'N', '1'}
		extern uint8_t NODE_HOST[];  // node host

		// ROUTER
		extern uint8_t ROUTER_GATEWAY[];  // router gateway
		extern uint8_t SUBNET_MASK[];  // of the router
		extern const char PASSWORD[];  // WiFi password
		extern const char SSID[];  // WiFi name
	}  // end namespace Config::Network
}  // end namespace Configure
