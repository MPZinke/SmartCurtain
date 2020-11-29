

#ifndef _GPIO_
#define _GPIO_

#include "Global.h"

namespace GPIO
{

	// ————————————————————————————————————————————————— GPIO: GLOBAL —————————————————————————————————————————————————

	const uint8_t CLOSE_PIN = 0;
	const uint8_t DIRECTION_PIN = 0;
	const uint8_t ENABLE_PIN = 0;
	const uint8_t OPEN_PIN = 0;
	const uint8_t PULSE_PIN = 0;

	const uint8_t HALL_EFFECT_IGNORED_PRECISION = 4;
	const uint8_t MIN_HALL_EFFECT_TRUE_VALUE = 255 >> HALL_EFFECT_IGNORED_PRECISION;  // min needed to be "ON"
	const uint16_t PULSE_WAIT = 16;
	const bool SWITCH = true;  // true = LOW is ON or false = HIGH is ON

	// ———— SUGAR ————
	const bool ON = HIGH ^ SWITCH;  // the "ON"/"ACTIVATE" state for the device
	const bool OFF = !ON;  // the "OFF"/"DEACTIVATE" state for the device


	// ————————————————————————————————————————————————— ABSTRACTIONS —————————————————————————————————————————————————

	void disable_motor()
	{
		digitalWrite(ENABLE_PIN, ON);
	}


	void enable_motor()
	{
		digitalWrite(ENABLE_PIN, OFF);
	}


	bool is_closed()
	{
		return analogRead(CLOSE_PIN) >> HALL_EFFECT_IGNORED_PRECISION > MIN_HALL_EFFECT_TRUE_VALUE;
	}


	bool is_open()
	{
		return analogRead(OPEN_PIN) >> HALL_EFFECT_IGNORED_PRECISION > MIN_HALL_EFFECT_TRUE_VALUE;
	}


	// ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	Curtain::CurtainState state()
	{
		if(is_open()) return Curtain::OPEN;
		if(is_closed()) return Curtain::CLOSED;
		return Curtain::MIDDLE;
	}


	// ——————————————————————————————————————————————————— MOVEMENT ———————————————————————————————————————————————————

	bool move(Curtain::Curtain& curtain, bool count_steps=false)
	{
		if(curtain.correct())
		{
			return false;  //TODO
		}
		else
		{
			return true;
		}
	}






	// —————————————————— SUGAR —————————————————



	void move_until_closed(bool curtain_direction)
	{
	// 	digitalWrite(DIRECTION_PIN, move_direction);
	// 	while(digitalRead(CLOSE_PIN))
	// 	{
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 	}
	}


	// —————————————————— TRACKING ————————————————

	uint32_t calibrate_to_opposite(bool direction)
	{
		// uint8_t opposing_stop_pin = !digitalRead(CLOSE_PIN) ? OPEN_PIN : CLOSE_PIN;

		// digitalWrite(DIRECTION_PIN, (opposing_stop_pin == OPEN_PIN) ^ raw_direction_bool);
		// return count_number_of_steps_to_pin(opposing_stop_pin);
		return 1;
	}


	// uint32_t count_number_of_steps_to_pin(uint8_t opposing_stop_pin)
	// {
	// 	register uint32_t step_count = 0;
	// 	while(digitalRead(opposing_stop_pin))
	// 	{
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		step_count += 2;
	// 	}
	// 	return step_count;
	// }


	// ————————————————— HARDWARE ————————————————


	// void move_until_closed(bool move_direction)
	// {
	// 	digitalWrite(DIRECTION_PIN, move_direction);
	// 	while(digitalRead(CLOSE_PIN))
	// 	{
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 	}
	// }


	void move_until_open(bool move_direction)
	{
	// 	digitalWrite(DIRECTION_PIN, move_direction);
	// 	while(digitalRead(OPEN_PIN))
	// 	{
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, HIGH);
	// 		delayMicroseconds(PULSE_WAIT);
	// 		digitalWrite(PULSE_PIN, LOW);
	// 		delayMicroseconds(PULSE_WAIT);
	// 	}
	}

}  // end namespace GPIO

#endif
