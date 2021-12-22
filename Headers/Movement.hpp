
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


#include <stdint.h>


namespace Event
{
	class Event;  // Declare it so it knows it exists
}


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
	}


	typedef uint8_t CurtainState;

	namespace CurtainStates
	{
		extern const uint8_t MIDDLE;
		extern const uint8_t CLOSE;
		extern const uint8_t CLOSED;  // alias of CLOSE for sugar
		extern const uint8_t OPEN;
	}


	// ———— POSITIONING ———— //
	inline bool is_approximate_position(uint32_t position1, uint32_t position2, uint32_t allowable_difference);
	inline bool is_approximate_position(uint32_t position1, uint32_t position2);
	inline CurtainState approximate_state_of(uint32_t position, uint32_t curtain_length);
	CurtainState state_of(uint8_t percentage);
	// inline CurtainState state_of(unsigned char percentage);
	inline CurtainState state_of(uint32_t position);
	inline CurtainState state_of(uint32_t position, uint32_t curtain_length);
	inline CurtainState state_of_position();
	// ———— GPIO ———— //
	inline void pulse_twice();
	inline void disable_motor();
	inline void enable_motor();
	inline void set_direction(CurtainState direction);
	// ———— STATE ———— //
	CurtainState current_state();
	inline bool endstop_triggered();
	inline bool is_closed();
	inline bool is_open();
	bool (*function_for_side(CurtainState state))();
	// ———— MOVEMENT ———— //
	void move(Event::Event& event);
	void reset();
	void auto_calibrate(Event::Event& event);
	uint32_t steps(Event::Event& event);  // Determine steps

	void step(const bool direction, register uint32_t steps);
	void step(register uint32_t steps);
	uint32_t step_and_count_to_position_or_end(bool(*state_function)());
	uint32_t step_down_to_position_or_end(register uint32_t steps, CurtainState direction);
	uint32_t step_down_to_position_or_end(register uint32_t steps, bool(*state_function)());





	uint32_t steps_for_direction(bool direction, uint32_t current_position, uint32_t desired_position);
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
