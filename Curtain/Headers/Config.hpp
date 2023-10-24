
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
	namespace Curtain
	{
		extern const uint16_t CURTAIN_ID;

		extern const uint32_t IGNORE_MOVEMENT_SIMILARITY;  // max step difference to ignore event
		extern const uint32_t POSITION_TOLLERANCE;  // steps within ends to consider "close enough"
		extern const uint32_t STEPS_FOR_CALIBRATION;  // how picky the program should be to commence recalibration
	}


	namespace Hardware
	{
		// ——————————————————————————————————————— HARDWARE DICTATING  VALUES ——————————————————————————————————————— //
		// —————————————————————————————————————————————————————————————————————————————————————————————————————————— //

		// ———————————————————————————————————— HARDWARE DICTATING  VALUES::PINS ———————————————————————————————————— //
		extern const uint8_t DIRECTION_PIN;
		extern const uint8_t ENABLE_PIN;
		extern const uint8_t PULSE_PIN;

		extern const uint8_t CLOSE_PIN;

		// ———————————————————————————————————— HARDWARE DICTATING  VALUES::PINS ———————————————————————————————————— //
		extern const uint16_t PULSE_WAIT;  // Wait between pulses for the stepper motor


		// ——————————————————————————————————————— HARDWARE DETERMINED VALUES ——————————————————————————————————————— //
		// —————————————————————————————————————————————————————————————————————————————————————————————————————————— //


		// ————————————————————————————————————— PHYSICAL HARDWARE BASED VALUES ————————————————————————————————————— //
		// These are determined by the hardware, as opposed to the hardware functionality being determined by these.  //
		// —————————————————————————————————————————————————————————————————————————————————————————————————————————— //
		extern const uint32_t MAX_LENGTH;

		// ———————————————————————————————— PHYSICAL HARDWARE BASED VALUES::SWITCHES ———————————————————————————————— //
		extern const bool DIRECTION_SWITCH;  // Used to make OPEN == ON and CLOSE == OFF.
		extern const bool SWITCH;  // true = LOW is ON or false = HIGH is ON (depends on electronic current directions)
	}


	namespace HTTP
	{
		extern const char AUTHORIZATION_VALUE[];
		extern const uint32_t AUTH_VALUE_LENGTH;
	}


	namespace Network
	{
		namespace MQTT
		{
			extern const char BROKER_DOMAIN[32];
			extern const char USERNAME[24];
			extern const char PASSWORD[24];
			extern const uint16_t PORT;
		}

		// Network
		extern const uint8_t MAC_ADDRESS[];

		namespace WiFi
		{
			extern const char PASSWORD[32];
			extern const char SSID[32];
		}
	}  // end namespace Config::Network
}  // end namespace Configure
