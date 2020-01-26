

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

#include"Definitions.h"
#include"ManualMove.h"
#include"DBGettersAndSetters.h"

TaskHandle_t manual_move_task;

void setup()
{
	Serial.begin(9600);
	Serial.write("\n\n----------------------SETTING UP ARDUINO----------------------\n\n");
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

			if(curtain_event_travels_full_span(event.event_position, details.curtain_length)) {
				Serial.write("\nFull span travel\n");
				reset_curtain_length(move_to_opposite_and_calibrate_curtain(details.curtain_direction));
			}
			else {
				Serial.write("\nSingle position travel\n");
				move_curtain(&details, event.event_position);
			}

			if(set_event_as_activated(event.event_id)) Serial.write("Successfully updated\n");
			else  Serial.write("Failed to update");
			set_curtain_as_deactivated();
			disable_motor();
		}
		else 
		{
			Serial.write(String(details.curtain_length).c_str());  //TESTING
			Serial.write("\n");
		}
		Serial.write("End activation\n\n");
	}

	delay(LOOP_WAIT);
}


// ————————————————— ACTIVATIONS —————————————————

bool curtain_event_travels_full_span(long destination, long length)
{
	if(!digitalRead(CLOSE_PIN) && length - 5 <  destination) return true;
	if(!digitalRead(OPEN_PIN) && destination < 5) return true;
	return false;
}


int move_curtain(CURTAIN_DETAILS* details, long event_position)
{
	bool move_direction = direction(details->curtain_position, event_position, details->curtain_direction);
	int remaining_steps = 0;

	if(event_position < 5) {
		Serial.write("Less than 5\n");
		move_until_close(move_direction);
		set_curtain_as_closed();
	}
	else if(details->curtain_length-5 < event_position) {
		Serial.write("Greater than 5\n");
		move_until_open(move_direction);
		set_curtain_as_open();
	}
	else
	{
		Serial.write("Where ever\n");
		int move_steps = needed_steps(details->curtain_position, event_position);
		int stop_pin = event_position < details->curtain_position ? CLOSE_PIN : OPEN_PIN;

		remaining_steps = step_motor_to_location_or_end(move_direction, move_steps, stop_pin);
		Serial.write("Remaining steps: ");
		Serial.write(String(remaining_steps).c_str());
		Serial.write("\n");
		if(remaining_steps)
		{
			if(stop_pin == CLOSE_PIN) set_curtain_as_closed();
			else set_curtain_as_open();
		}
		//TODO: assume length and move to originally desired position
	}

	return remaining_steps;
}
