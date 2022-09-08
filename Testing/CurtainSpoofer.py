

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

		self.add_url_rule("/", "/", self.index, ["GET", "POST"])


	def serve(self):
		while(True):
			self.run(host="0.0.0.0", port=self._port)


	def index(self):
		global CURTAIN, FLAG;
		body = request.json;
		FLAG = True;

		if(body.get("query type") == "status"):
			return dict(CURTAIN);

		if(body.get("query type") == "move"):
			CURTAIN.percentage(body.get("event", {}).get("percentage", 0))
			return {"success":"Moving to position"};

		print(body);
		return {"error": "Your Test Failed"};


def receive_flag():
	global CURTAIN, FLAG;

	while(True):
		if(FLAG):
			sleep(4);
			FLAG = False;

			requests.get("http://google.com")  #TODO
		sleep(1)


def main():
	global CURTAIN, FLAG
	CURTAIN = Curtain()
	FLAG = False

	app = App()
	curtain_thread = Thread(name="receive_flag", target=receive_flag)
	app_thread = Thread(name="App", target=app.serve)
	app_thread.start()
	curtain_thread.start()


if __name__ == '__main__':
	main()
