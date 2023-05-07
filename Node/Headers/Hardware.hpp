
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
		extern const CurtainState CLOSE;
		extern const CurtainState CLOSED;  // alias of CLOSE for sugar
		extern const CurtainState OPEN;
	}


	void disable_motor();
	void enable_motor();
	void pulse();
	void set_direction(CurtainState direction);
	bool endstop_triggered();
	bool is_closed();
	CurtainState state();
}  // end namespace Hardware
