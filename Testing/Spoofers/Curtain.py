#!/opt/homebrew/bin/python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2022.09.18                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


from flask import Flask, request
import requests
import sys
from threading import Thread
from time import sleep



class Curtain:
	def __init__(self, *, id: int=1, auto_calibrate: bool=False, auto_correct: bool=True, direction: bool=False,
	  discrete_movement: bool=False, length=32000, percentage: int=0, position: int=0):
		self._id = id;
		self._auto_calibrate = auto_calibrate;
		self._auto_correct = auto_correct;
		self._direction = direction;
		self._discrete_movement = discrete_movement;
		self._length = length;
		self._percentage = percentage;
		self._position = position;


	def __iter__(self) -> dict:
		yield from {
			"id": self._id,
			"auto calibrate": self._auto_calibrate,
			"auto correct": self._auto_correct,
			"direction": self._direction,
			"discrete movement": self._discrete_movement,
			"length": self._length,
			"percentage": self._percentage,
			"position": self._position,
		}.items()


	def percentage(self, percentage=None):
		if(percentage is not None):
			self._percentage = percentage;
			self._position = int(percentage * self._length / 100);

		else:
			return self._percentage;


class App(Flask):
	def __init__(self, name: str=__name__):
		Flask.__init__(self, name)
		self._port = int(sys.argv[1]) if(len(sys.argv) > 1) else 8081

		self.add_url_rule("/", "/", self.index, methods=["GET", "POST"])


	def serve(self):
		while(True):
			self.run(host="0.0.0.0", port=self._port)


	def index(self):
		global CURTAIN, ACTION;
		body = request.json;

		if(body.get("query type") == "status"):
			return dict(CURTAIN);

		if(body.get("query type") == "move"):
			CURTAIN.percentage(body.get("event", {}).get("percentage", 0));
			ACTION = move
			return {"success":"Moving to position"};

		print(body);
		return {"error": "Your Test Failed"};


def move():
	global CURTAIN;

	sleep(CURTAIN._length / 8000);
	requests.patch(f"http://localhost:8080/api/v1.0/curtains/{CURTAIN._id}", json={"is_activated": False});


def execute_action():
	global CURTAIN, ACTION;

	while(True):
		sleep(1)
		if(ACTION is None):
			continue;

		try:
			ACTION();
		except Exception as error:
			print(error);
		ACTION = None;


def main():
	global CURTAIN, ACTION
	CURTAIN = Curtain();
	ACTION = None;

	app = App()
	curtain_thread = Thread(name="execute_action", target=execute_action)
	app_thread = Thread(name="App", target=app.serve)
	app_thread.start()
	curtain_thread.start()


if __name__ == '__main__':
	main()
