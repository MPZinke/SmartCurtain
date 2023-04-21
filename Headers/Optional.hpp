

class Optional
{

	private:
		StaticJsonDocument<JSON_BUFFER_SIZE> _value;
		bool _ok = true;

	public:
		Optional()
		: _ok{false}
		{}

		Optional(StaticJsonDocument<JSON_BUFFER_SIZE> value)
		: _value{value}
		{}

		bool ok()
		{
			return _ok;
		}

		StaticJsonDocument<JSON_BUFFER_SIZE> value()
		{
			return _value;
		}
};
