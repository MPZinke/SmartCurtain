

class DeserializedJSON
{
	private:
		StaticJsonDocument<JSON_BUFFER_SIZE> _document;
		bool _ok;

	public:
		Optional(String json)
		{
			_ok = !deserializeJson(_document, json);
		}

		bool ok()
		{
			return _ok;
		}

		StaticJsonDocument<JSON_BUFFER_SIZE> value()
		{
			return _value;
		}
};
