

namespace Curtain
{
	using namespace Movement::CurtainStates;


	Curtain::Curtain()
	{}


	// —————————————————————————————————————————————— GETTERS  —————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————— GETTERS::STRUCTURE ————————————————————————————————————————— //

	uint16_t id()
	{
		return _id;
	}


	uint16_t room_id()
	{
		return _room_id;
	}


	uint16_t home_id()
	{
		return _home_id;
	}


	// —————————————————————————————————————————— GETTERS::OPTIONS —————————————————————————————————————————— //

	bool auto_correct()
	{
		return _auto_correct;
	}


	// ————————————————————————————————————————— GETTERS::HARDWARE  ————————————————————————————————————————— //

	uint32_t length()
	{
		return _length;
	}


	bool direction()
	{
		return _direction;
	}


	bool is_moving()
	{
		return _is_moving;
	}


	uint32_t percentage()
	{
		return _position * 100 / _length;
	}


	// ——————————————————————————————————————————— GETTERS::OTHER ——————————————————————————————————————————— //

	CurtainState Curtain::state()
	{
		if(Hardware::state() == CLOSED)
		{
			_position = 0;
		}
		else if(_position == 0)
		{
			_position = _length;
		}

		if(_position == 0)
		{
			return CLOSED;
		}
		else if(_position == _length)
		{
			return FULLY_OPEN;
		}

		return PARTIALLY_OPEN;
	}


	operator String()
	{
		// 
	}


	inline String invalid_key_message(const char* key, const char* type_str)
	{
		return String("Curtain object must contain key '") + key + "' of type '" + type_str + "'";
	}


	static bool Curtain::validate(StaticJsonDocument<JSON_BUFFER_SIZE>& json_document)
	{
		// Validate structure
		if(!json_document.containsKey(CURTAIN_ID) || !json_document[CURTAIN_ID].is<int>())
		{
			new BAD_REQUEST_400_Exception(__FILE__, __LINE__, invalid_key_message(CURTAIN_ID, "int"));
			return false;
		}

		else if(json_document.containsKey(HOME_ID) && !json_document[HOME_ID].is<int>())
		{
			new BAD_REQUEST_400_Exception(__FILE__, __LINE__, invalid_key_message(HOME_ID, "int"));
			return false;
		}

		else if(json_document.containsKey(ROOM_ID) && !json_document[ROOM_ID].is<int>())
		{
			new BAD_REQUEST_400_Exception(__FILE__, __LINE__, invalid_key_message(ROOM_ID, "int"));
			return false;
		}

		// Validate hardware overriding values
		else if(json_document.containsKey(DIRECTION) && !json_document[DIRECTION].is<bool>())
		{
			new BAD_REQUEST_400_Exception(__FILE__, __LINE__, invalid_key_message(DIRECTION, "bool"));
			return false;
		}

		else if(json_document.containsKey(LENGTH) && !json_document[LENGTH].is<int>())
		{
			new BAD_REQUEST_400_Exception(__FILE__, __LINE__, invalid_key_message(LENGTH, "int"));
			return false;
		}

		// Validate movement overriding values
		else if(json_document.containsKey(AUTO_CORRECT) && !json_document[AUTO_CORRECT].is<bool>())
		{
			new BAD_REQUEST_400_Exception(__FILE__, __LINE__, invalid_key_message(AUTO_CORRECT, "bool"));
			return false;
		}

		return true;
	}


	// —————————————————————————————————————————————— SETTERS  —————————————————————————————————————————————— //
	// —————————————————————————————————————————————————————————————————————————————————————————————————————— //

	// ————————————————————————————————————————— SETTERS::STRUCTURE ————————————————————————————————————————— //

	void room_id(uint16_t new_room_id)
	{
		_room_id = new_room_id
	}


	void home_id(uint16_t new_home_id)
	{
		_home_id = new_home_id
	}


	// ————————————————————————————————————————— SETTERS::HARDWARE  ————————————————————————————————————————— //

	void direction(bool new_direction)
	{
		_direction = new_direction
	}


	void length(uint32_t new_length)
	{
		_length = new_length
	}


	void is_moving(bool new_is_moving)
	{
		_is_moving = new_is_moving
	}


	void percentage(uint32_t new_percentage)
	{
		_position = new_percentage * _length / 100;
	}


	// —————————————————————————————————————————— SETTERS::OPTIONS —————————————————————————————————————————— //

	void auto_correct(bool new_auto_correct)
	{
		_auto_correct = new_auto_correct
	}


	void update()
	{
		if(Hardware::state() == CLOSED)
		{
			_position = 0;
		}
		else if(_position == 0)
		{
			_position = _length;
		}
	}


	void update(DeserializedJSON& update_json)
	{

	}
}
