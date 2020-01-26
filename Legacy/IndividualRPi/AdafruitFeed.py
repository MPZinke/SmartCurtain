#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

###########################################################################
#
#	created by: MPZinke
#	on ..
#
#	DESCRIPTION:
#	BUGS:
#	FUTURE:
#
###########################################################################


from Adafruit_IO import MQTTClient
from time import sleep, time
from threading import Thread

from Definitions import *
import DBFunctions
import ErrorWriter
import SetCurtain

# —————————————————— UTILITY ——————————————————–

def steps_to_take(cursor, event_position):
	total_steps = DBFunctions.curtain_length(cursor)
	event_position_steps = total_steps * event_position / 100
	return DBFunctions.current_position() - event_position_steps


# —————————————— ADAFRUIT CLIENT —————————————————

def connect_to_feeds(client):
	for feed in [OPEN_KEY, CLOSE_KEY]:
		client.subscribe(feed)


def disconnect(client):
	raise Exception("MQTTClient disconnected")


def feed_actions(client, feed_id, payload):
	from GPIOUtility import motor_is_engaged
	if motor_is_engaged(): return  # motor is currently active; ignore change

	cnx, cursor = DBFunctions.connect_to_DB()

	if feed_id == OPEN_KEY:
		try: payload = int(payload)
		except: return
		steps = steps_to_take(cursor, payload)
		SetCurtain.activate(cnx, cursor, steps)
	elif feed_id == CLOSE_KEY:
		SetCurtain.close_curtain(1 ^ DBFunctions.direction(cursor))

	cnx.close()


def active_feed(client):
	client.connect()
	client.loop_blocking()


def feed_loop(client):
	while True:
		cnx, cursor = DBFunctions.connect_to_DB()

		feed_option_is_selected =  DBFunctions.adafruit_feed(cursor) 
		if feed_option_is_selected and not client.is_connected():
			thread = Thread(target=activate_feed, args=(client,))
			thread.start()
			thread.join()
		elif not feed_option_is_selected and client.is_connected():
			client.disconnect()

		sleep(FEED_CLIENT_CHECK_LOOP)


def start_client_loop():
	client = MQTTClient(FEED_NAME, FEED_KEY)

	client.on_connect = connect_to_feeds
	client.on_disconnect = disconnect
	client.on_message = feed_actions

	feed_loop(client)
