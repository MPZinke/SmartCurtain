
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
		const uint8_t CLOSE = (uint8_t)CurrentPull::CLOSE;
		const uint8_t CLOSED = CLOSE;  // alias for sugar
		const uint8_t OPEN = (uint8_t)CurrentPull::OPEN;
	}


	using namespace CurtainStates;  // used CurtainStates as enum


	void disable_motor()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		digitalWrite(ENABLE_PIN, CurrentPull::OFF);
	}


	void enable_motor()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		digitalWrite(ENABLE_PIN, CurrentPull::ON);
	}


	// Pulse motor (HIGH->LOW).
	void pulse()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		digitalWrite(PULSE_PIN, HIGH);
		delayMicroseconds(PULSE_WAIT);
		digitalWrite(PULSE_PIN, LOW);
		delayMicroseconds(PULSE_WAIT);
	}


	// Sets the direction pin of the motor for the stepper driver.
	// Take the ON/OFF dirction current, the curtain's direction option. 
	void set_direction(CurtainState direction)
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		// Curtain direction can overflow 0th bit to act as a switch. 
		digitalWrite(DIRECTION_PIN, ((direction + Global::curtain.direction()) & 0b1));
	}


	// ————————————————————————————————————————————————— GPIO::READ ————————————————————————————————————————————————— //

	bool endstop_triggered()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return is_closed() == CurrentPull::ON;
	}


	bool is_closed()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		return digitalRead(CLOSE_PIN) == CurrentPull::ON;
	}


	// —————————————————————————————————————————————— POSITION::STATE  —————————————————————————————————————————————— //

	// Gets the state (in form CurtainState) of the curtain based on hardware.
	CurtainState state()
	/*
	SUMMARY: 
	PARAMS:  
	DETAILS: 
	RETURNS: 
	*/
	{
		if(is_closed())
		{
			return CLOSED;
		}

		return OPEN;
	}
}  // end namespace Hardware
