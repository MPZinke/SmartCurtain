#!/opt/homebrew/bin/python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2023.04.07                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


import json
import re
from typing import Optional, TypeVar


from SmartCurtain import Option
from SmartCurtain import AreaOption
from SmartCurtain import Curtain


Home = TypeVar("Home")
Room = TypeVar("Room")


class Room:
	def __init__(self, Home: Optional[Home]=None, *, id: int, is_deleted: bool, name: str,
	  RoomOptions: list[AreaOption[Room]], Curtains: list[Curtain]
	):
		# STRUCTURE #
		self._Home = Home
		# DATABASE #
		self._id: int = id
		self._is_deleted: bool = is_deleted
		self._name: str = name
		self._RoomOptions: list[AreaOption[Room]] = RoomOptions.copy()
		self._Curtains: list[Curtain] = Curtains.copy()

		[room_option.Room(self) for room_option in self._RoomOptions]
		[curtain.Room(self) for curtain in self._Curtains]


	@staticmethod
	def from_dictionary(room_data: dict) -> Room:
		curtains: list[Curtain] = [Curtain.from_dictionary(curtain_data) for curtain_data in room_data["Curtains"]]

		options: list = []
		for option_data in room_data["RoomsOptions"]:
			options.append(AreaOption[Room](**{**option_data, "Option": Option(**option_data["Option"])}))

		return Room(id=room_data["id"], is_deleted=room_data["is_deleted"], name=room_data["name"],
		  RoomOptions=options, Curtains=curtains)


	# —————————————————————————————————————————————— GETTERS & SETTERS  —————————————————————————————————————————————— #
	# ———————————————————————————————————————————————————————————————————————————————————————————————————————————————— #

	def __getitem__(self, Curtain_id: int) -> Optional[Curtain]:
		return next((room for room in self._Curtains if(room.id() == Curtain_id)), None)


	def __iter__(self) -> dict:
		yield from {
			"id": self._id,
			"is_deleted": self._is_deleted,
			"name": self._name,
			"RoomOptions": list(map(dict, self._RoomOptions)),
			"Curtains": list(map(dict, self._Curtains))
		}.items()


	def __repr__(self) -> str:
		return str(self)


	def __str__(self) -> str:
		return json.dumps(dict(self), default=str, indent=4)


	def id(self):
		return self._id


	def is_deleted(self, new_is_deleted: Optional[bool]=None) -> Optional[bool]:
		if(new_is_deleted is None):
			return self._is_deleted

		if(not isinstance(new_is_deleted, bool)):
			raise Exception(f"'Room::is_deleted' must be of type 'bool' not '{type(new_is_deleted).__name__}'")

		self._is_deleted = new_is_deleted


	def name(self, new_name: Optional[str]=None) -> Optional[str]:
		if(new_name is None):
			return self._name

		if(not isinstance(new_name, str)):
			raise Exception(f"'Room::name' must be of type '{str.__name__}' not '{type(new_name).__name__}'")

		self._name = new_name


	def RoomOption(self, Option_id: int) -> Optional[AreaOption[Room]]:
		return next((option for option in self._RoomOptions if(option.Option().id() == Option_id)), None)


	def RoomOptions(self) -> list[AreaOption[Room]]:
		return self._RoomOptions.copy()


	# ————————————————————————————————————————— GETTERS & SETTERS::CHILDREN  ————————————————————————————————————————— #

	def Curtains(self):
		return self._Curtains.copy()


	# —————————————————————————————————————————— GETTERS & SETTERS::PARENTS —————————————————————————————————————————— #

	def Home(self, new_Home: Optional[Home]=None) -> Optional[Home]:
		if(new_Home is None):
			return self._Home

		from SmartCurtain import Home
		if(not isinstance(new_Home, Home)):
			raise Exception(f"'Room::Home' must be of type 'Home' not '{type(new_Home).__name__}'")

		self._Home = new_Home


	# ————————————————————————————————————————————————————— MQTT ————————————————————————————————————————————————————— #

	def path(self) -> str:
		path_name = re.sub(r"[^_\-a-zA-Z0-9]", "", self._name)
		return f"{self._Home.path()}/{path_name}"