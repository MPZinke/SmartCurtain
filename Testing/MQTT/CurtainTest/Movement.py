

from typing import TypeVar


from . import Global
from .Hardware import CLOSE, OPEN
from . import Hardware
from . import Message


Event = TypeVar("Event")


def move(event: Event) -> None:
	if(Global.curtain.is_moving() == False):
		return

	Global.curtain.update()
	if(event.percentage() != Global.curtain.percentage()):
		if(event.percentage() == 0):
			Secure.move_until_closed()
		elif(event.direction() == CLOSE):
			if(not Secure.move_towards_closed(event.steps())):
				if(not Global.curtain.auto_correct()):
					event.percentage(0)
				else:
					Global.curtain.update()
					Hardware.set_direction(OPEN)
					Unsecure.step(event.steps())
		else:
			Hardware.set_direction(event.direction())
			Unsecure.step(event.steps())

	Global.curtain.percentage(event.percentage())
	Global.curtain.is_moving(False)
	Message.update_hub()


class Secure:
	@staticmethod
	def move_and_count_to_closed() -> int:
		Hardware.set_direction(CLOSE)

		Hardware.enable_motor()

		for steps_not_taken in range(0xFFFFFFFF, 0, -1):
			if(Hardware.is_closed()):
				break

			Hardware.pulse()

		Hardware.disable_motor()

		return 0xFFFFFFFF - steps_not_taken


	@staticmethod
	def move_towards_closed(remaining_steps: int) -> bool:
		Hardware.set_direction(CLOSE)
		Hardware.enable_motor()

		for remaining_steps in range(remaining_steps, 0, -1):
			if(Hardware.is_closed()):
				break

			Hardware.pulse()

		Hardware.disable_motor()
		return remaining_steps-1 == 0


	@staticmethod
	def move_until_closed() -> None:
		Hardware.enable_motor()

		for _ in range(0xFFFFFFFF):
			if(Hardware.is_closed()):
				break

			Hardware.pulse()

		Hardware.disable_motor()


class Unsecure:
	@staticmethod
	def step(steps: int) -> None:
		Hardware.enable_motor()

		for _ in range(steps, 0, -1):
			Hardware.pulse()

		Hardware.disable_motor()
