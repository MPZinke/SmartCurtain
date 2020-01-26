
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


#ifndef MANUALMOVE_H
#define MANUALMOVE_H

	#include"Definitions.h"
	#include"DBGettersAndSetters.h"
	#include"GPIO.h"


	void manual_move_loop(void* args)
	{
		while(true)
		{	
			if(current_activated_pin() == CLOSE_PIN)
				set_curtain_as_closed();
			else if(current_activated_pin() == OPEN_PIN)
				set_curtain_as_open();
			else set_curtain_as_partially_open();

			delay(MANUAL_MOVEMENT_CHECK_WAIT);
		}
	}

#endif
