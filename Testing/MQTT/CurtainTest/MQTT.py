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
import paho.mqtt.client as mqtt


class MQTTClient(mqtt.Client):
	def __init__(self):
		mqtt.Client.__init__(self)

		self._Global_client_message = None
		self._Global_client_topic = None


	def on_connect(self, client, userdata, flags, result_code) -> None:
		import Global

		print(f"Connected with result code {str(result_code)}")
		self.subscribe("SmartCurtain/all/move")
		self.subscribe("SmartCurtain/all/status")
		self.subscribe(f"""SmartCurtain/-/-/{Global.curtain.id()}/move""")
		self.subscribe(f"""SmartCurtain/-/-/{Global.curtain.id()}/status""")
		self.subscribe(f"""SmartCurtain/-/-/{Global.curtain.id()}/update""")
		client.publish("SmartCurtain/hub/update", str(Global.curtain))

	
	def messageTopic(self) -> str:
		return self._Global_client_topic


	def on_message(self, client, userdata, message):
		self._Global_client_message = message.payload
		self._Global_client_topic = message.topic

		print("DEBUG", message.payload)

		import Control
		Control.process_message(len(message.payload))
