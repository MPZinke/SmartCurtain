

from typing import Optional


from . import Global
from .Hardware import CLOSE, OPEN


class Event:
	def __init__(self, event_json: dict):
		self._percentage = event_json["percentage"]


	def direction(self) -> str:
		return {False: OPEN, True: CLOSE}[self._percentage < Global.curtain.percentage()]


	def percentage(self, new_percentage: Optional[int]=None) -> Optional[int]:
		if(new_percentage is None):
			return self._percentage

		self._percentage = new_percentage


	def steps(self) -> int:
		steps_delta = self._percentage - Global.curtain.percentage()
		if(steps_delta < 0):
			steps_delta *= -1

		return int(steps_delta * Global.curtain.length() / 100)
