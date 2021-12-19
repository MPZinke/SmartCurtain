
/***********************************************************************************************************************
*                                                                                                                      *
*   created by: MPZinke                                                                                                *
*   on 2020.11.28                                                                                                      *
*                                                                                                                      *
*   DESCRIPTION: Handware control part of the code. The trickiest part of this page is the XOR—direction and ON/OFF    *
*       are depenent on physical setup of the motor, and the High/Low of the stepper driver.                           *
*   CONVENTION: - OPEN = ON, CLOSE = OFF                                                                               *
*   BUGS:                                                                                                              *
*   FUTURE:                                                                                                            *
*                                                                                                                      *
***********************************************************************************************************************/


#ifndef __MOVEMENT__
#define __MOVEMENT__


#include "Global.hpp"

#include "C_String.hpp"


namespace Movement
{

	// ————————————————————————————————————————————————— GPIO: GLOBAL —————————————————————————————————————————————————

	// ———— SUGAR ————
	namespace CurrentPull
	{
		extern const bool ON;  // the "ON"/"ACTIVATE" state for the device
		extern const bool OFF;  // the "OFF"/"DEACTIVATE" state for the device

		extern const bool CLOSE;  // solidify convention
		extern const bool OPEN;  // solidify convention

		extern bool DIRECTION_SWITCH;
	}


	namespace CurtainStates
	{
		extern const uint8_t MIDDLE;
		extern const uint8_t CLOSE;
		extern const uint8_t CLOSED;  // alias of CLOSE for sugar
		extern const uint8_t OPEN;
	}


	typedef uint8_t CurtainState;


	inline void pulse_twice();
	void disable_motor();
	void enable_motor();
	void set_direction(bool direction_current);
	CurtainState current_state();
	inline bool endstop_triggered();
	inline bool is_closed();
	inline bool is_open();
	bool (*function_for_side(bool open_close_value))();
	uint32_t steps_for_direction(bool direction, uint32_t current_position, uint32_t desired_position);
	void move_steps(const bool direction, register uint32_t steps);
	void move_steps(register uint32_t steps);
	void move_until_state_reached(bool(*state_function)());
	uint32_t move_and_count_until_state_reached(bool(*state_function)()=endstop_triggered);
	bool sensor_triggered_moving_steps(register uint32_t steps, bool(*state_function)()=endstop_triggered);
	uint32_t set_direction_move_and_count_until_state_reached(const bool direction,
	  bool(*state_function)()=endstop_triggered);
	void initial_positioning();
	void move_until_closed();
	void move_until_open();
	uint32_t calibrate_to_opposite(bool curtain_direction);

}  // end namespace GPIO


#endif
