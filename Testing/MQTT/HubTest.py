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


class HubConnection(mqtt.Client):
	def __init__(self):
		mqtt.Client.__init__(self)

		self.curtains = {}


	def on_connect(self, client, userdata, flags, result_code) -> None:
		print(f"Connected with result code {str(result_code)}")
		self.subscribe("curtains/hub")
		self.publish("curtains/all", """{"type": "status"}""")


	def on_message(self, client, userdata, message) -> None:
		curtain_info: dict = json.loads(message.payload)
		self.curtains[curtain_info["id"]] = curtain_info
		print(self.curtains)


	def run(self) -> None:
		self.connect("localhost", 1883, 60)
		self.loop_forever()


def main():
	hub_connection = HubConnection()
	hub_connection.run()


if(__name__ == "__main__"):
	main()
