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


UPDATE_DATA = """{"type": "register", "Curtain": {"Home": 1, "Room": 1}}"""


class HubConnection(mqtt.Client):
	def __init__(self):
		mqtt.Client.__init__(self)

		self.curtains = {}


	def on_connect(self, client, userdata, flags, result_code) -> None:
		print(f"Connected with result code {str(result_code)}")
		self.subscribe("SmartCurtain/hub/update")
		self.subscribe("SmartCurtain/hub/error")
		self.publish("SmartCurtain/all/status", "")


	def on_message(self, client, userdata, message) -> None:
		# Status: The request for something's (a curtain's) status
		# Updates: Specify the data to update.
		# HUB ——status--> Curtain ——update--> Hub [——update--> Curtain]
		#	IE. Hub: "What is your status?", Curtain: "Let me update you"[, Hub: "Let me tweak that"]
		# Curtain ——update--> Hub [——update--> Curtain]
		#	IE. Curtain: "This is what you should show for me"[, Hub: "Let me tweak that"]
		# The Hub can however override on home, room, length, and other DB defined values.
		# However, the Hub will not override is_moving or percentage
		print(request := json.loads(message.payload))
		print(type := message.topic.split("/")[-1])

		if(type == "update"):
			if((curtain := request["Curtain"])["id"] not in self.curtains):
				# Testing "is_connected" and updating values from DB.
				self.curtains[curtain["id"]] = {**curtain, "home": 1, "room": 1, "length": curtain["length"]-1}

			self_curtain = self.curtains[curtain["id"]]
			self_curtain.update({key: curtain[key] for key in curtain if(key in ["is_moving", "percentage"])})

			if(any(self_curtain[key] != curtain[key] for key in ["home", "room", "length", "Auto Correct"])):
				self.publish(f"""SmartCurtain/-/-/{curtain["id"]}/update""", json.dumps({"Curtain": self_curtain}))

		elif(type == "error"):
			print(f"Error received: {message.payload}")


	def run(self) -> None:
		self.connect("localhost", 1883, 60)
		self.loop_forever()


def main():
	hub_connection = HubConnection()
	hub_connection.run()


if(__name__ == "__main__"):
	main()
