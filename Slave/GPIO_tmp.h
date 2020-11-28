

#ifndef _GPIO_
#define _GPIO_

#include "Global.h"

namespace GPIO
{

	const uint16_t PULSE_WAIT = 16;


	// —————————————————— SUGAR —————————————————

	void disable_motor()
	{
		digitalWrite(ENABLE_PIN, HIGH);
	}


	void enable_motor()
	{
		digitalWrite(ENABLE_PIN, LOW);
	}


	void move_until_closed(bool curtain_direction)
	{
		digitalWrite(DIRECTION_PIN, move_direction);
		while(digitalRead(CLOSE_PIN))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
		}
	}


	CurtainState state()
	{
		if(is_open()) return OPEN;
		if(is_closed()) return CLOSED;
	}

	// —————————————————— TRACKING ————————————————

	void calibrate_moving_to_opposite(bool raw_direction)
	{
		uint8_t opposing_stop_pin = !digitalRead(CLOSE_PIN) ? OPEN_PIN : CLOSE_PIN;

		digitalWrite(DIRECTION_PIN, (opposing_stop_pin == OPEN_PIN) ^ raw_direction_bool);
		return count_number_of_steps_to_pin(opposing_stop_pin);
	}


	uint32_t count_number_of_steps_to_pin(uint8_t opposing_stop_pin)
	{
		register uint32_t step_count = 0;
		while(digitalRead(opposing_stop_pin))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			step_count += 2;
		}
		return step_count;
	}


	// ————————————————— HARDWARE ————————————————


	void move_until_closed(bool move_direction)
	{
		digitalWrite(DIRECTION_PIN, move_direction);
		while(digitalRead(CLOSE_PIN))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
		}
	}


	void move_until_open(bool move_direction)
	{
		digitalWrite(DIRECTION_PIN, move_direction);
		while(digitalRead(OPEN_PIN))
		{
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, HIGH);
			delayMicroseconds(PULSE_WAIT);
			digitalWrite(PULSE_PIN, LOW);
			delayMicroseconds(PULSE_WAIT);
		}
	}

}  // end namespace GPIO

#endif