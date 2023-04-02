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
		self.length = length
		self.is_moving = False


	def __iter__(self) -> dict:
		yield from {
			"id": self.id,
			"length": self.length,
			"is_moving": self.is_moving
		}.items()


	def on_connect(self, client, userdata, flags, result_code) -> None:
		print(f"Connected with result code {str(result_code)}")
		self.subscribe("curtains/all")
		self.subscribe(f"curtains/{self.id}")
		client.publish("curtains/hub", json.dumps(dict(self)))


	def on_message(self, client, userdata, message) -> None:
		print(message.payload)  #TESTING
		request = json.loads(message.payload)
		if(request["type"] == "move"):
			self.is_moving = True
			client.publish("curtains/hub", json.dumps(dict(self)))
			sleep(3)
			self.is_moving = False

		client.publish("curtains/hub", json.dumps(dict(self)))


	def run(self) -> None:
		self.connect("localhost", 1883, 60)
		self.loop_forever()


def main():
	id = sys.argv[1]
	curtain_connection = CurtainConnection(id, 52000)
	curtain_connection.run()


if(__name__ == "__main__"):
	main()
