
/***********************************************************************************************************
*
*	created by: MPZinke
*	on ..
*
*	DESCRIPTION: As par https://techtutorialsx.com/2017/05/20/esp32-http-post-requests/
*	BUGS:
*	FUTURE:
*
***********************************************************************************************************/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

	#include <HTTPClient.h>
	#include <WiFi.h>

	// ———— USER ————

	#define CURTAIN_NUMBER 1

	// ———— WIFI ————
	#define SSID "Zinc_Home"
	#define WIFI_PASSWORD "WelcomeToZinke123!@#"

	#define REQUEST_ADDRESS "http://10.0.0.11/connections/API.php"

	// ———— GPIO ————
	#define ENABLE_PIN '\32'  // char version of int 26
	#define PULSE_PIN '\33'  // char version of int 27
	#define DIRECTION_PIN '\41'  // char version of int 33
	// #define DIRECTION_PIN 33
	// #define ENABLE_PIN 26
	// #define PULSE_PIN 27

	#define CLOSE_PIN '\15'  // char version of int 13
	#define OPEN_PIN '\16'  // char version of int 14

	#define LED_BUILTIN '\2'

	// ———— WAIT ————
	#define HTTP_FAIL_RESPONSE_WAIT 5000
	#define LOOP_WAIT 500
	#define MANUAL_MOVEMENT_CHECK_WAIT 10000
	#define PULSE_WAIT 1

	// ———— OTHER ————
	#define IGNORE_MOVEMENT_SIMILARITY 10  // max step difference to ignore event
	#define STEPS_FOR_CALIBRATION 5  // how picky the program should be movement

	// ———— CONCAT STRING TO CURTAIN_NUMBER ———— 
	#define NUM_STR(x) #x
	#define STR(x, y) #x NUM_STR(y)
	#define STR_LNG(x, y, z) #x NUM_STR(y) #z


	enum CURTAIN_STATES
	{
		CLOSED,
		INBETWEEN,
		OPEN
	};


	typedef struct CURTAIN_DETAILS
	{
		char curtain_direction;
		long curtain_length;
		long curtain_position;
	} CURTAIN_DETAILS;


	typedef struct EVENT
	{
		int event_id;
		long event_position;
	} EVENT;

#endif
