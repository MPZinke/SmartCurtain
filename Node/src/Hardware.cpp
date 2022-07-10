
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2022.03.14                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: TEMPLATE                                                                                              *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#include "Arduino.h"


#include "../Headers/Hardware.hpp"

#include "../Headers/Config.hpp"
#include "../Headers/Global.hpp"
#include "../Headers/Curtain.hpp"


namespace Hardware
{
	using namespace Config::Hardware;


	// ———— SUGAR ————
	namespace CurrentPull
	{
		const bool ON = HIGH ^ SWITCH;  // the "ON"/"ACTIVATE" state for the device
		const bool OFF = !ON;  // the "OFF"/"DEACTIVATE" state for the device

		const bool CLOSE = OFF;  // solidify convention
		const bool OPEN = !CLOSE;  // solidify convention
	}


	namespace CurtainStates
	{
		const uint8_t UNKNOWN = -2;  // technically 0xFE
		const uint8_t MIDDLE = -1;  // technically 0xFF
		const uint8_t CLOSE = (uint8_t)CurrentPull::CLOSE;
		const uint8_t CLOSED = CLOSE;  // alias for sugar
		const uint8_t OPEN = (uint8_t)CurrentPull::OPEN;
	}


	using namespace CurtainStates;  // used CurtainStates as enum


	void disable_motor()
	{
		digitalWrite(ENABLE_PIN, CurrentPull::ON);
	}


	void enable_motor()
	{
		digitalWrite(ENABLE_PIN, CurrentPull::OFF);
	}


	// Pulse motor (HIGH->LOW) twice.
	void pulse_twice()
	{
		digitalWrite(PULSE_PIN, HIGH);
		delayMicroseconds(PULSE_WAIT);
		digitalWrite(PULSE_PIN, LOW);
		delayMicroseconds(PULSE_WAIT);

		digitalWrite(PULSE_PIN, HIGH);
		delayMicroseconds(PULSE_WAIT);
		digitalWrite(PULSE_PIN, LOW);
		delayMicroseconds(PULSE_WAIT);
	}


	// Sets the direction pin of the motor for the stepper driver.
	// Take the ON/OFF dirction current, the curtain's direction option. 
	void set_direction(CurtainState direction)
	{
		// Curtain direction can overflow 0th bit to act as a switch. 
		digitalWrite(DIRECTION_PIN, ((direction + Global::curtain.direction()) & 0b1));
	}


	// ————————————————————————————————————————————————— GPIO::READ ————————————————————————————————————————————————— //

	bool endstop_triggered()
	{
		return is_closed() || is_open();
	}


	bool is_closed()
	{
		return digitalRead(CLOSE_PIN);
	}


	bool is_open()
	{
		return digitalRead(OPEN_PIN);
	}


	// —————————————————————————————————————————————— POSITION::STATE  —————————————————————————————————————————————— //

	// Gets the state (in form CurtainState) of the curtain based on hardware.
	CurtainState current_hardware_state()
	{
		if(OPEN_ENDSTOP && is_open()) return OPEN;
		else if(CLOSE_ENDSTOP && is_closed()) return CLOSE;
		else if(BOTH_ENDSTOPS) return MIDDLE;
		else return UNKNOWN;
	}
}  // end namespace Hardware