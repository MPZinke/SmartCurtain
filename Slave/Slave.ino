

/***********************************************************************************************************
*
*	created by: MPZinke
*	on 01.18.19
*
*	DESCRIPTION: As par https://techtutorialsx.com/2017/05/20/esp32-http-post-requests/
*		Used for ESP32 arduino dev board.
*		Code for slave unit of smart curtain.  Creates connection to wifi network to post to &
*		receive json packets from API.  In Definitions.h set `curtain_id` under 
*		CURTAIN_NUMBER, network name under SSID, password under WIFI_PASSWORD,
*		& the API address under REQUEST_ADDRESS
*	BUGS:
*	FUTURE:
*
***********************************************************************************************************/

#include"Definitions.h"
#include"ManualMove.h"
#include"DBGettersAndSetters.h"

TaskHandle_t manual_move_task;

void setup()
{
	pinMode(CLOSE_PIN, INPUT);
	pinMode(OPEN_PIN, INPUT);
	pinMode(DIRECTION_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);
	pinMode(PULSE_PIN, OUTPUT);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);

	delay(4000);  // delay necessary
 
	WiFi.begin(SSID, WIFI_PASSWORD); 
	while(WiFi.status() != WL_CONNECTED) delay(1000);  // check connection

	xTaskCreatePinnedToCore(	manual_move_loop, "Manual Move Checker", 10000, 
									NULL, 1, &manual_move_task, 0);
}


void loop()
{
	digitalWrite(LED_BUILTIN, LOW);
	delay(250);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(250);
	digitalWrite(LED_BUILTIN, LOW);

	disable_motor();  // default to manual movement allowed
	EVENT event;
	get_most_recent_event(&event);

	if(event.event_id)
	{
		CURTAIN_DETAILS details;
		get_curtain_details(&details);

		// check that curtain has been set up
		if(details.curtain_length) 
		{
			// prepare for movement
			set_curtain_as_activated();
			enable_motor();

			if(curtain_event_travels_full_span(event.event_position, details.curtain_length))
			{
				if(auto_calibration_option())
					reset_curtain_length(move_to_opposite_and_calibrate_curtain(details.curtain_direction));
				else
				{
					if(current_activated_pin() == CLOSE_PIN)
						move_until_open(direction(CLOSE_PIN, details.curtain_direction));
					else move_until_close(direction(OPEN_PIN, details.curtain_direction));
				}
			}
			else if(position_not_reached_after_moving(&details, event.event_position) && auto_correct_option())
				assume_length_and_move_to_desired_position(&details, event.event_position);

			set_event_as_activated(event.event_id);
			set_curtain_as_deactivated();
			disable_motor();
		}
	}

	delay(LOOP_WAIT);
}


// ————————————————— ACTIVATIONS —————————————————

void assume_length_and_move_to_desired_position(CURTAIN_DETAILS* details, long event_position)
{
	char current_pin = current_activated_pin();
	step_motor_to_location_or_end(direction(current_pin, details->curtain_direction), 
		needed_steps(current_pin, details->curtain_length, event_position), 
		current_pin == CLOSE_PIN ? OPEN_PIN : CLOSE_PIN);
}


int position_not_reached_after_moving(CURTAIN_DETAILS* details, long event_position)
{
	bool move_direction = direction(	details->curtain_position,
										event_position,
										details->curtain_direction);

	if(event_position < 5)
	{
		move_until_close(move_direction);
		set_curtain_as_closed();
	}
	else if(details->curtain_length-5 < event_position)
	{
		move_until_open(move_direction);
		set_curtain_as_open();
	}
	else
	{
		long move_steps = needed_steps(details->curtain_position, event_position);
		char stop_pin = event_position < details->curtain_position ? CLOSE_PIN : OPEN_PIN;
		return step_motor_to_location_or_end(move_direction, move_steps, stop_pin);
	}

	return 0;
}


// —————————————————— UTILITY ———————————————————

bool curtain_event_travels_full_span(long destination, long length)
{
	if(!digitalRead(CLOSE_PIN) && length - 5 <  destination) return true;
	if(!digitalRead(OPEN_PIN) && destination < 5) return true;
	return false;
}


bool direction(long current_position, long event_position, bool DB_move_direction)
{
	return (0 < (current_position - event_position)) ^ DB_move_direction;
}


bool direction(char current_pin, bool DB_move_direction)
{
	(current_pin == OPEN_PIN) ^ DB_move_direction;
}


long needed_steps(long current_position, long event_position)
{
	return abs(event_position - current_position);
}


long needed_steps(char current_pin, long curtain_length, long event_position)
{
	if(current_pin == CLOSE_PIN) return event_position;
	return curtain_length - event_position;
}