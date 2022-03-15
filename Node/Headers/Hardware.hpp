
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


#include <stdint.h>


namespace Hardware
{
	// ———— SUGAR ————
	typedef uint8_t CurtainState;


	namespace CurrentPull
	{
		extern const bool ON;  // the "ON"/"ACTIVATE" state for the device
		extern const bool OFF;  // the "OFF"/"DEACTIVATE" state for the device

		extern const bool CLOSE;  // solidify convention
		extern const bool OPEN;  // solidify convention
	}


	namespace CurtainStates
	{
		extern const CurtainState UNKNOWN;
		extern const CurtainState MIDDLE;
		extern const CurtainState CLOSE;
		extern const CurtainState CLOSED;  // alias of CLOSE for sugar
		extern const CurtainState OPEN;
	}


	void disable_motor();
	void enable_motor();
	void pulse_twice();
	void set_direction(CurtainState direction);
	bool endstop_triggered();
	bool is_closed();
	bool is_open();
	CurtainState current_hardware_state();
}  // end namespace Hardware
