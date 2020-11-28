
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.26                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Ethernet connection version of smart curtain.  Queries server and waits                               *
*		on response for next event.  On event, drives curtain to location.                                             *
*		**Main arduino file**                                                                                          *
*   BUGS:                                                                                                              *
*   FUTURE: - Add cool temperature, light, & thermostat data.                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

// #include <SPI.h>
// #include <Ethernet.h>

#include "Global.h"
#include "Transmissions.h"


IPAddress SERVER(MASTER_HOST_ARRAY);
EthernetClient CLIENT;  // the magician

btye PACKET_BUFFER[PACKET_LENGTH+1];


void setup()
{
	// GPIO setup
	pinMode(CLOSE_PIN, INPUT);
	pinMode(OPEN_PIN, INPUT);
	pinMode(DIRECTION_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);
	pinMode(PULSE_PIN, OUTPUT);

	disable_motor();  // don't burn up the motor

	// ———— GLOBAL VARIABLES ————
	PACKET_BUFFER[PACKET_LENGTH] = 0;  // make sure buffer does not ever read out of bound
	// ethernet setup
	Ethernet.init();  // defaults to 10 (Teensy 3.2, etc)
	while(!Ethernet.begin(CURTAIN_MAC)) delay(1000);  // wait while not connected to LAN
	while(!CLIENT.connect(SERVER, 80)) delay(1000);  // wait while not connected to device


	delay(LOOP_WAIT);
}


void loop()
{
	post_data("curtain=" DEF(CURTAIN_NUMBER));
	if(read_response_successfully_into_buffer())
	{
		Transmission transmission = decode_response();
	}

}


