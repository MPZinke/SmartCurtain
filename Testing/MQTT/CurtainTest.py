#!/opt/homebrew/bin/python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2023.04.02                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


import json
import paho.mqtt.client as mqtt
import sys
from time import sleep


class CurtainConnection(mqtt.Client):
	def __init__(self, id, length):
		mqtt.Client.__init__(self)

		self.id = int(id)

		self.home = 0
		self.room = 0
		self.auto_correct = False
		self.length = length

		self.is_moving = False
		self.percentage = 0


	def __iter__(self) -> dict:
		yield from {
			"id": self.id,
			"Home.id": self.home,
			"Room.id": self.room,
			"Auto Correct": self.auto_correct,
			"length": self.length,
			"is_moving": self.is_moving,
			"percentage": self.percentage,
		}.items()


	def on_connect(self, client, userdata, flags, result_code) -> None:
		print(f"Connected with result code {str(result_code)}")
		self.subscribe("SmartCurtain/all/move")
		self.subscribe("SmartCurtain/all/status")
		self.subscribe(f"""SmartCurtain/-/-/{self.id}/move""")
		self.subscribe(f"""SmartCurtain/-/-/{self.id}/status""")
		self.subscribe(f"""SmartCurtain/-/-/{self.id}/update""")
		client.publish("SmartCurtain/hub/update", json.dumps(dict(self)))


	def on_message(self, client, userdata, message) -> None:
		print(type := message.topic.split("/")[-1])

		if(type == "move"):
			print(request := json.loads(message.payload))
			self.move(request)

		if(type == "status"):
			print(json.dumps(dict(self)))
			self.publish("SmartCurtain/hub/update", json.dumps(dict(self)))

		if(type == "update"):
			print(request := json.loads(message.payload))
			self.update(request)
			print(json.dumps(dict(self)))
			self.publish("SmartCurtain/hub/update", json.dumps(dict(self)))


	def move(self, request):
		print(request)

		self.is_moving = True
		client.publish("SmartCurtain", json.dumps(dict(self)))

		sleep(3)

		self.percentage = 100 * int(not bool(self.percentage))
		self.is_moving = False

		self.publish("SmartCurtain/hub/update", json.dumps({"Curtain": dict(self)}))


	def update(self, request):
		if(self.home != request["Home.id"]):
			if(self.home != 0):
				self.unsubscribe(f"""SmartCurtain/{self.home}/status""")
				self.unsubscribe(f"""SmartCurtain/{self.home}/move""")
			self.home = request["Home.id"]
			self.subscribe(f"""SmartCurtain/{self.home}/status""")
			self.subscribe(f"""SmartCurtain/{self.home}/move""")

		if(self.room != request["Room.id"]):
			if(self.room != 0):
				self.unsubscribe(f"""SmartCurtain/-/{self.room}/status""")
				self.unsubscribe(f"""SmartCurtain/-/{self.room}/move""")
			self.room = request["Room.id"]
			self.subscribe(f"""SmartCurtain/-/{self.room}/status""")
			self.subscribe(f"""SmartCurtain/-/{self.room}/move""")

		if("length" in request and request["length"] < self.length):
			self.length = request["length"]

		if("Auto Correct" in request and request["Auto Correct"] < self.auto_correct):
			self.auto_correct = request["Auto Correct"]


	def run(self) -> None:
		self.connect("localhost", 1883, 60)
		self.loop_forever()


def main():
	id = sys.argv[1]
	curtain_connection = CurtainConnection(id, 55000)
	curtain_connection.run()


if(__name__ == "__main__"):
	main()
