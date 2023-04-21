

namespace Curtain
{
	using namespace Movement::CurtainStates;


	uint16_t Curtain::id()
	{
		return _id;
	}


	uint16_t Curtain::room_id()
	{
		return _room_id;
	}


	uint16_t Curtain::home_id()
	{
		return _home_id;
	}


	bool Curtain::auto_correct()
	{
		return _auto_correct;
	}


	bool Curtain::direction()
	{
		return _direction;
	}


	uint32_t Curtain::length()
	{
		return _length;
	}


	uint32_t Curtain::percentage()
	{
		return _position * 100 / _length;
	}


	CurtainState Curtain::state()
	{
		if(_position == 0)
		{
			return CLOSED;
		}
		if(_position == _length)
		{
			return FULLY_OPEN;
		}

		return PARTIALLY_OPEN;
	}



	void Curtain::room_id(uint16_t new_room_id)
	{
		_room_id = new_room_id;
	}


	void Curtain::home_id(uint16_t new_home_id)
	{
		_home_id = new_home_id;
	}


	void Curtain::auto_correct(bool new_auto_correct)
	{
		_auto_correct = new_auto_correct;
	}


	void Curtain::direction(bool new_direction)
	{
		_direction = new_direction;
	}


	void Curtain::length(uint32_t new_length)
	{
		_length = new_length;
	}


	void Curtain::percentage(uint32_t new_percentage)
	{
		_position = new_percentage * _length / 100;
	}
}
