#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2020.12.19                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


from datetime import datetime, timedelta;

from Class.DBClass import DBClass;
from DB.DBCredentials import *;
from DB.DBFunctions import __CLOSE__, __CONNECT__;
from DB.DBFunctions import SELECT_CurtainsEvent, SELECT_current_CurtainsEvents, SELECT_CurtainsOptions;
from Other.Logger import log_error;
from System.CurtainsEvents import CurtainsEvents;
from System.CurtainsOptions import CurtainsOptions;


class Curtains(DBClass):
	def __init__(self, **curtain_info):
		DBClass.__init__(self, "set_Curtain", **curtain_info);

		# Get associated relations
		cnx, cursor = __CONNECT__(DB_USER, DB_PASSWORD, DATABASE);

		current_events = SELECT_current_CurtainsEvents(cursor, self._id)
		self._CurtainsEvents = {event["id"]: CurtainsEvents(**{**event, "Curtain": self}) for event in current_events};

		curtains_options = SELECT_CurtainsOptions(cursor, self._id);
		self._CurtainsOptions = {option["Options.id"] : CurtainsOptions(**option) for option in curtains_options};

		__CLOSE__(cnx, cursor);


	# ———————————————————————————————————————————————— GETTERS/SETTERS ————————————————————————————————————————————————
	# —————————————————————————————————————————————————————————————————————————————————————————————————————————————————

	# ——————————————————————————————————— GETTERS/SETTERS::DB COLUMN SIMPLE QUERIES ———————————————————————————————————

	def id(self) -> int:
		return self._id;


	# ———————————————————————————————————————————————— GETTERS: OBJECTS ————————————————————————————————————————————————

	def CurtainsEvents(self) -> dict:
		return self._CurtainsEvents;


	def CurtainsOption(self, CurtainsOptions_id : int=None):
		return self._CurtainsOptions.get(CurtainsOptions_id);


	def CurtainsOptions(self):
		return self._CurtainsOptions;


	def System(self):
		return self._System;


	# ———————————————————————————————————————————————— GETTERS: SPECIAL ————————————————————————————————————————————————

	# Get all curtain events for a time range.
	# Takes the latest datetime time that an event can be, optionally the earliest datetime time an event can be.
	# Cycles through dictionary of events. If an event is within the range, event is added to list.
	# Returns list of curtain events within that time range.
	def CurtainsEvents_for_range(self, latest : object=None, earliest : object=None) -> list:
		if(not earliest and not latest): return [self._CurtainsEvents[event_id] for event_id in self._CurtainsEvents];
		events = [];
		for event_id in self._CurtainsEvents:
			event = self._CurtainsEvents[event_id];
			if((latest and latest < event.time()) or (earliest and event.time() < earliest)): continue;
			events.append(event);
		return events;


	
	def prior_CurtainsEvents_for_current_day_of_week(self, earliest : object=None) -> list:
		earliest = earliest or datetime.today() - timedelta(days=28);

		cnx, cursor = __CONNECT__(DB_USER, DB_PASSWORD, DATABASE);
		CurtainsEvents_data = SELECT_CurtainsEvent(cursor, CurtainsEvents_id);
		__CLOSE__(cnx, cursor);


	# Get CurtainsEvent if exists.
	# Takes the CurtainsEvents id to pull from.
	# Checks whether the CurtainsEvents exists in memory. If it doesn't, checks if it exists in the DB.
	# Returns the Event if it is found, else None.
	def CurtainsEvent(self, CurtainsEvents_id : int=None):
		if(CurtainsEvents_id in self._CurtainsEvents): return self._CurtainsEvents.get(CurtainsEvents_id);  # easy!

		# not found, check if in DB
		cnx, cursor = __CONNECT__(DB_USER, DB_PASSWORD, DATABASE);
		CurtainsEvents_data = SELECT_CurtainsEvent(cursor, CurtainsEvents_id);
		__CLOSE__(cnx, cursor);

		# return if found in DB
		if(CurtainsEvents_data): event = CurtainsEvents(**{**CurtainsEvents_data, "Curtain": self});
		if(not CurtainsEvents_data or event.Curtains_id() != self._id): return None;
		self._CurtainsEvents[event.id()] = event;
		return event;



	# ———————————————————————————————————————————————————— UTILITY ————————————————————————————————————————————————————

	def dict(self, desired_attrs=[]):
		ignore_attrs = ["_CurtainsEvents", "_CurtainsOptions"];
		if(desired_attrs): return {attr : getattr(self, attr)() for attr in desired_attrs if attr not in ignore_attrs};

		attrs = ["_id", "_current_position", "_direction", "_is_activated", "_last_connection", "_length", "_name"];
		class_dict = {attr : getattr(self, attr) for attr in attrs};
		class_dict["_CurtainsEvents"] = {ce : self._CurtainsEvents[cd].dict() for ce in self._CurtainsEvents};
		class_dict["_CurtainsOptions"] = {co : self._CurtainsOptions[co].dict() for co in self._CurtainsOptions};
		return class_dict;


	def print(self, tab=0, next_tab=0):
		attrs = ["_id", "_current_position", "_direction", "_is_activated", "_last_connection", "_length", "_name"];
		for attr in attrs: print('\t'*tab, attr, " : ", getattr(self, attr));
		print('\t'*tab, "_CurtainsEvents : ");
		for event in self._CurtainsEvents: self._CurtainsEvents[event].print(tab+next_tab, next_tab);
		print('\t'*tab, "_CurtainsOptions : ");
		for option in self._CurtainsOptions: self._CurtainsOptions[option].print(tab+next_tab, next_tab);


	# ——————————————————————————————————————————————————————— UI ———————————————————————————————————————————————————————

	def current_position_percent_float(self) -> float:
		return 100 / self._length * self._current_position;


	def current_position_percent_int(self) -> int:
		return int(100 / self._length * self._current_position);


	def state_string(self) -> str:
		if(self._is_activated): return "Moving";
		return {0 : "Closed", self._length : "Fully Open"}.get(self._current_position, "Open");


	# ——————————————————————————————————————————————————————— DB ———————————————————————————————————————————————————————

	def _new_event(self, *, desired_position: int=0, Options_id: int=None, time: object=None) -> int:
		if(isinstance(time, type(None))): time = datetime.now();

		kwargs = {"Curtain": self, "Options.id": Options_id, "desired_position": desired_position, "time": time};
		new_CurtainEvents = CurtainsEvents.New(**kwargs);
		self._CurtainsEvents[new_CurtainEvents.id()] = new_CurtainEvents;

		return new_CurtainEvents.id();


	def close(self, *, Options_id : int=None, time : object=None):
		if(isinstance(time, type(None))): time = datetime.now();

		return self._new_event(desired_position=0, Options_id=Options_id, time=time);


	def close_immediately(self, Options_id : int=None) -> int:
		return self._new_event(Options_id=Options_id);


	def open(self, *, desired_position : int=0, Options_id : int=None, time : object=None) -> int:
		if(isinstance(time, type(None))): time = datetime.now();

		return self._new_event(desired_position=desired_position, Options_id=Options_id, time=time);


	def open_immediately(self, desired_position : int=0, Options_id : int=None) -> int:
		CurtainsEvents_id = self._new_event(desired_position=desired_position, Options_id=Options_id);
		return CurtainsEvents_id if CurtainsEvents_id else False;


	def open_percentage(self, *, desired_position : int=0, Options_id : int=None, time : object=None) -> int:
		if(isinstance(time, type(None))): time = datetime.now();

		desired_position = int(desired_position * self._length / 100);
		return self._new_event(Options_id=Options_id, desired_position=desired_position, time=time);
