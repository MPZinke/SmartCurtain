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

from time import sleep

from Definitions import *
import ErrorWriter


# —————————————————— SUNSET ——————————————————

def sunset_time():
	from astral import Astral

	astr_object = Astral()
	astr_object.solar_depression = "civil"
	try: return astr_object[CITY].sun(local=True)["sunset"]
	except Exception as error:
		ErrorWriter.write_error(error)
		return None


def sunset_loop():
	from DBFunctions import 	add_event, connect_to_DB, \
								event_set_at_approximate_time, sunset_close

	while True:
		cnx, cursor = connect_to_DB()

		if sunset_close(cursor):
			sunset = sunset_time()
			if is_null_sleep_then(sunset): continue
			# check if sunset event already set
			if not event_set_at_approximate_time(cursor, 0, sunset):
				add_event(cnx, cursor, 0, sunset)

		sleep(SUNSET_LOOP_WAIT)

		cnx.close()


# —————————————————— SUNRISE ——————————————————

def sunrise_time():
	from astral import Astral

	astr_object = Astral()
	astr_object.solar_depression = "civil"
	try: return astr_object[CITY].sun(local=True)["sunrise"]
	except Exception as error:
		ErrorWriter.write_error(error)
		return None


def sunrise_loop():
	from DBFunctions import 	add_event, connect_to_DB, curtain_length, \
								event_set_at_approximate_time, sunrise_open

	while True:
		cnx, cursor = connect_to_DB()

		if sunrise_open(cursor):
			sunrise = sunrise_time()
			if is_null_sleep_then(sunrise): continue
			open_position = curtain_length(cursor)
			if not event_set_at_approximate_time(cursor, open_position, sunrise):
				add_event(cnx, cursor, open_position, sunrise)

		sleep(SUNRISE_LOOP_WAIT)

		cnx.close()
