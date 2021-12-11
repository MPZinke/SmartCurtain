
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


#pragma once


#include <stdint.h>


namespace Config
{
	namespace Curtain
	{
		const char CURTAIN_ID[] = "1";

		const uint32_t IGNORE_MOVEMENT_SIMILARITY = 10;  // max step difference to ignore event
		const uint32_t POSITION_TOLLERANCE = 5;  // steps within ends to consider "close enough"
		const uint32_t STEPS_FOR_CALIBRATION = 5;  // how picky the program should be to commence recalibration
	}


	namespace Hardware
	{
		// ———————— HARDWARE DETERMINED VALUES ————————
		#define CLOSE_ENDSTOP true
		#define ENCODER true
		#define OPEN_ENDSTOP false

		// These are determined by the hardware, as opposed to the hardware functionality being determined by these.
		// ———— STEPS ————
		const uint32_t DEFAULT_LENGTH = 34000;
		const uint16_t ENCODER_STEPS_PER_ROTATION = 800;
		const uint16_t MOTOR_STEPS_PER_ROTATION = 3200;  // 200 st/rot * 16 microstepping

		// ———— SWITCHES ————
		const bool DIRECTION_SWITCH = false;  // Used to make OPEN == ON and CLOSE == OFF.
		const bool SWITCH = false;  // true = LOW is ON or false = HIGH is ON (depends on electronic current directions)

		// ———————— HARDWARE DICTATING VALUES ————————
		// ———— PINS ————
		const uint8_t DIRECTION_PIN = 5;
		const uint8_t ENABLE_PIN = 18;
		const uint8_t PULSE_PIN = 19;

		const uint8_t CLOSE_PIN = 0;
		const uint8_t ENCODER_PINA = 0;
		const uint8_t ENCODER_PINB = 0;
		const uint8_t OPEN_PIN = 0;

		// ———— WAIT ————
		const uint16_t ENCODER_WAIT = 60;  // Wait between reads of the encoder.
		const uint16_t PULSE_WAIT = 60;  // Wait between pulses for the stepper motor
	}


	namespace Network
	{
		// NODE
		const uint16_t PORT = 80;  // listening port
		uint8_t MAC_ADDRESS[] = {0xDE, 0x43, 0x52, 0x54, 0x4E, 0x31};  // {'Z', 'C', 'R', 'T', 'N', '1'}
		uint8_t NODE_HOST[] = {10,0,0,12};  // node host

		// HUB
		uint8_t HUB_HOST[] = {10, 0, 0, 11};  // hub host IP
		const char HUB_HOST_STR[] = "10.0.0.11";  // hub host cstr

		// ROUTER
		uint8_t ROUTER_GATEWAY[] = {10,0,0,1};  // router gateway
		uint8_t SUBNET_MASK[] = {255,255,255,0};  // of the router
		const char PASSWORD[] = "";  // WiFi password
		const char SSID[] = "";  // WiFi name
	}  // end namespace Config::Network


	namespace Transmission
	{
		const uint8_t ACTION_COMPLETE_URL[] = "/api/update/deactivateevent";
	}
}  // end namespace Configure
