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

		self.id = id
		self.home = 0
		self.room = 0
		self.length = length

		self.is_moving = False
		self.percentage = 0


	def __iter__(self) -> dict:
		yield from {
			"id": self.id,
			"home": self.home,
			"room": self.room,
			"length": self.length,
			"is_moving": self.is_moving,
			"percentage": self.percentage,
		}.items()


	def on_connect(self, client, userdata, flags, result_code) -> None:
		print(f"Connected with result code {str(result_code)}")
		self.subscribe("SmartCurtain/all")
		self.subscribe(f"""SmartCurtain/-/-/{self.id}""")
		client.publish("SmartCurtain/hub", json.dumps({"type": "update", "Curtain": dict(self)}))


	def on_message(self, client, userdata, message) -> None:
		print(request := json.loads(message.payload))

		if(request["type"] == "status"):
			self.publish("SmartCurtain/hub", json.dumps({"type": "update", "Curtain": dict(self)}))

		if(request["type"] == "update"):
			self.update(request)

		if(request["type"] == "move"):
			self.move(request)


	def move(self, request):
		print(request)

		self.is_moving = True
		client.publish("SmartCurtain", json.dumps(dict(self)))

		sleep(3)

		self.percentage = 100 * int(not bool(self.percentage))
		self.is_moving = False

		self.publish("SmartCurtain/hub", json.dumps({"type": "update", "Curtain": dict(self)}))


	def update(self, request):
		self.home = request["Curtain"]["home"]
		self.room = request["Curtain"]["room"]
		self.subscribe(f"""SmartCurtain/{self.home}""")
		self.subscribe(f"""SmartCurtain/-/{self.room}""")

		if("length" in request["Curtain"]):
			self.length = request["Curtain"]["length"]


	def run(self) -> None:
		self.connect("localhost", 1883, 60)
		self.loop_forever()


def main():
	id = sys.argv[1]
	curtain_connection = CurtainConnection(id, 52000)
	curtain_connection.run()


if(__name__ == "__main__"):
	main()
