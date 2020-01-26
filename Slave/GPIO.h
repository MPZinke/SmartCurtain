
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


#ifndef GPIO_H
#define GPIO_H

	#include"Definitions.h"

	bool direction(long, long, bool);
	void disable_motor();
	void enable_motor();
	long needed_steps(long, long);
	long count_number_of_steps();
	char current_activated_pin();
	bool motor_is_engages();
	void move_until_close(bool);
	void move_direction_until_end_reached(bool);
	void move_until_open(bool);
	int step_motor_to_location_or_end(bool, register long, register char);

	// —————————————————— SUGAR —————————————————

	void disable_motor()
	{
		digitalWrite(ENABLE_PIN, HIGH);
	}


	void enable_motor()
	{
		digitalWrite(ENABLE_PIN, LOW);
	}


	// ————————————————— HARDWARE ————————————————

	long count_number_of_steps_to_pin(char opposing_stop_pin)
	{
		register long step_count = 0;
		while(digitalRead(opposing_stop_pin))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			step_count += 2;
		}
		return step_count;
	}


	char current_activated_pin()
	{
		if(!digitalRead(CLOSE_PIN)) return CLOSE_PIN;
		if(!digitalRead(OPEN_PIN)) return OPEN_PIN;
		return 0;
	}


	bool motor_is_engages()
	{
		return digitalRead(ENABLE_PIN);
	}


	void move_direction_until_end_reached(bool move_direction)
	{
		digitalWrite(DIRECTION_PIN, move_direction);
		while(!digitalRead(CLOSE_PIN) && !digitalRead(OPEN_PIN))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
		}
	}


	long move_to_opposite_and_calibrate_curtain(bool raw_direction_bool)
	{
		char opposing_pin;

		if(!digitalRead(CLOSE_PIN))
		{
			digitalWrite(DIRECTION_PIN, true ^ raw_direction_bool);
			opposing_pin = OPEN_PIN;
		}
		else
		{
			digitalWrite(DIRECTION_PIN, false ^ raw_direction_bool);
			opposing_pin = CLOSE_PIN;
		}
		return count_number_of_steps_to_pin(opposing_pin);
	}


	void move_until_close(bool move_direction)
	{
		digitalWrite(DIRECTION_PIN, move_direction);
		while(digitalRead(CLOSE_PIN))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
		}
	}


	void move_until_open(bool move_direction)
	{
		digitalWrite(DIRECTION_PIN, move_direction);
		while(digitalRead(OPEN_PIN))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
		}
	}


	// int step_motor_to_location_or_end(const int const int move_steps, const int opposing_stop_pin)
	int step_motor_to_location_or_end(bool move_direction, register long move_steps, register char opposing_stop_pin)
	{
		Serial.write("step_motor_to_location_or_end\n");
		digitalWrite(DIRECTION_PIN, move_direction);
		while(0 < move_steps)
		{
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delay(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delay(PULSE_WAIT);
			move_steps -= 3;
			if(!digitalRead(opposing_stop_pin)) 
				return move_steps > STEPS_FOR_CALIBRATION ? move_steps : 0;
		}

		return 0;
	}

#endif
