
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


#ifndef __CONFIG__
#define __CONFIG__


#include <stdint.h>


#define JSON_BUFFER_SIZE 0x400


namespace Config
{
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
		extern const uint8_t ENCODER_PINA;
		extern const uint8_t ENCODER_PINB;
		extern const uint8_t OPEN_PIN;

		// ———— WAIT ————
		extern const uint16_t ENCODER_WAIT;  // Wait between reads of the encoder.
		extern const uint16_t PULSE_WAIT;  // Wait between pulses for the stepper motor

		// ———————— HARDWARE DETERMINED VALUES ————————
		extern const bool CLOSE_ENDSTOP;
		extern const bool OPEN_ENDSTOP;
		extern const bool ENCODER;
		extern const bool BOTH_ENDSTOPS;  // whether curtain contains both endstops

		// These are determined by the hardware, as opposed to the hardware functionality being determined by these.
		// ———— STEPS ————
		extern const uint32_t DEFAULT_LENGTH;
		extern const uint16_t ENCODER_STEPS_PER_ROTATION;
		extern const uint16_t MOTOR_STEPS_PER_ROTATION;  // 200 st/rot * 16 microstepping

		// ———— SWITCHES ————
		extern const bool DIRECTION_SWITCH;  // Used to make OPEN == 100 and CLOSE == 0.
		extern const bool SWITCH;  // true = LOW is ON or false = HIGH is ON (depends on electronic current directions)
	}


	namespace Network
	{
		// NODE
		extern const uint16_t PORT;  // listening port
		extern uint8_t MAC_ADDRESS[];  // {'Z', 'C', 'R', 'T', 'N', '1'}
		extern uint8_t NODE_HOST[];  // node host

		// HUB
		extern char HUB_HOST_STR[];  // hub host cstr

		// ROUTER
		extern uint8_t ROUTER_GATEWAY[];  // router gateway
		extern uint8_t SUBNET_MASK[];  // of the router
		extern const char PASSWORD[];  // WiFi password
		extern const char SSID[];  // WiFi name
	}  // end namespace Config::Network


	namespace Transmission
	{
		extern const uint8_t ACTION_COMPLETE_URL[];
	}
}  // end namespace Configure


#endif
