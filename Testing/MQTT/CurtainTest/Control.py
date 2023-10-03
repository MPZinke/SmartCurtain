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
import threading


from . import Global
from . import Event
from . import Movement


def process_message(message_size: int) -> None:
	type = Global.client.messageTopic().split("/")[-1]

	if(type == "move"):
		case_move(message_size)

	if(type == "status"):
		case_status(message_size)

	if(type == "update"):
		case_update(message_size)

	Global.client.publish("SmartCurtain/hub/update", str(Global.curtain))


def loop() -> None:
	Global.client.connect("localhost", 1883, 60)
	Global.client.loop_forever()


def case_move(message_size: int):
	if(Global.curtain.is_moving()):
		raise Exception("Curtain is already moving")

	event_json = json.loads(Global.client._Global_client_message)
	if("percentage" not in event_json):
		raise Exception("""Event json must be of format '{"percentage": 100}'""")

	event = Event.Event(event_json)
	Global.curtain.is_moving(True)
	thread = threading.Thread(name="Event thread", target=Movement.move, args=(event,))
	thread.start()


def case_status(message_size: int):
	Global.curtain.update()


def case_update(message_size: int):
	curtain_json = json.loads(Global.client._Global_client_message)
	Global.curtain += curtain_json
