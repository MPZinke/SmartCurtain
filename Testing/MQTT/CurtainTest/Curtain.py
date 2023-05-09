#!/opt/homebrew/bin/python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2023.05.08                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


import json
from typing import Optional


class Curtain:
	def __init__(self, id, length, virtual_physical_position=0, position=0):
		self._id = int(id)

		self._home = 0
		self._room = 0
		self._auto_correct = True
		self._length = length

		self._is_moving = False

		self._direction = True
		self._virtual_physical_position = virtual_physical_position  # What the truth of the hardware is
		self._position = position  # What the hardware thinks its state is


	def __iter__(self) -> dict:
		yield from {
			"id": self._id,
			"Home.id": self._home,
			"Room.id": self._room,
			"Auto Correct": self._auto_correct,
			"length": self._length,
			"is_moving": self._is_moving,
			"percentage": self.percentage(),
		}.items()


	def __repr__(self) -> str:
		return str(self)


	def __str__(self) -> str:
		return json.dumps(dict(self), indent=4)


	def id(self) -> int:
		return self._id


	def direction(self) -> bool:
		return self._direction


	def home(self) -> int:
		return self._home


	def room(self) -> int:
		return self._room


	def auto_correct(self, new_auto_correct: Optional[bool]) -> Optional[bool]:
		if(new_auto_correct is None):
			return self._auto_correct

		self._auto_correct = new_auto_correct


	def is_moving(self, new_is_moving: Optional[bool]=None) -> Optional[bool]:
		if(new_is_moving is None):
			return self._is_moving

		self._is_moving = new_is_moving


	def length(self, new_length: Optional[bool]=None) -> Optional[bool]:
		if(new_length is None):
			return self._length

		self._length = new_length if(new_length < self._length) else self._length


	def percentage(self, new_percentage: Optional[int]=None) -> Optional[int]:
		if(new_percentage is None):
			return int(self._position * 100 / self._length)

		self._position = new_percentage * self._length / 100


	def update(self) -> None:
		if(self._virtual_physical_position == 0):
			self._position = 0
		elif(self._position == 0 and self._virtual_physical_position != 0):
			self._position = 100


	def __iadd__(self, request: dict):
		import Global

		if(self._home != request["Home.id"]):
			if(self._home != 0):
				Global.client.unsubscribe(f"""SmartCurtain/{self._home}/status""")
				Global.client.unsubscribe(f"""SmartCurtain/{self._home}/move""")

			self._home = request["Home.id"]
			Global.client.subscribe(f"""SmartCurtain/{self._home}/status""")
			Global.client.subscribe(f"""SmartCurtain/{self._home}/move""")

		if(self._room != request["Room.id"]):
			if(self._room != 0):
				Global.client.unsubscribe(f"""SmartCurtain/-/{self._room}/status""")
				Global.client.unsubscribe(f"""SmartCurtain/-/{self._room}/move""")

			self._room = request["Room.id"]
			Global.client.subscribe(f"""SmartCurtain/-/{self._room}/status""")
			Global.client.subscribe(f"""SmartCurtain/-/{self._room}/move""")

		if("length" in request and request["length"] < self._length):
			self._length = request["length"]

		if("Auto Correct" in request):
			self._auto_correct = request["Auto Correct"]

		return self
