

namespace Curtain
{
	using Movement::CurtainState;


	class Curtain
	{
		private:
			uint16_t _id;
			uint16_t _room_id;
			uint16_t _home_id;

			bool _auto_correct;
			bool _direction;

			uint32_t _length;
			uint32_t _position;

		public:
			uint16_t id();
			uint16_t room_id();
			uint16_t home_id();

			bool auto_correct();
			bool direction();

			uint32_t length();
			uint32_t percentage();
			CurtainState state();


			void room_id(uint16_t new_room_id);
			void home_id(uint16_t new_home_id);

			void auto_correct(bool new_auto_correct);
			void direction(bool new_direction);

			void length(uint32_t new_length);
			void percentage(uint32_t new_position);
	};
}
