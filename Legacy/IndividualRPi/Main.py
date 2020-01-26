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

from threading import Thread
from time import sleep

from Definitions import *

import AdafruitFeed
import CalibrateCurtain
import DaytimeEvents
import DBFunctions as DBFun
import ErrorWriter
import EventPredictor
import GPIOUtility
import SetCurtain


# ——————————————— PRIMARY PROCESSES ——————————————

def check_and_run_any_pending_events(cnx, cursor):
	# get events [(event_key, desire_position)]
	non_activated_events = DBFun.all_non_activated_events(cursor)
	if non_activated_events:
		# run newest; ignore others
		remaining_steps = SetCurtain.activate_curtain(cnx, cursor, non_activated_events[0][1])
		for event in non_activated_events:
			DBFun.mark_event_as_activated(cnx, cursor, event[0])

		return remaining_steps
	return None


def primary_process_loop():
	# main process loop
	while True:
		cnx, cursor = DBFun.connect_to_DB()

		remaining_steps = check_and_run_any_pending_events(cnx, cursor)
		if remaining_steps and DBFun.auto_calibration():
			CalibrateCurtain.calibrate_if_necessary(cnx, cursor, remaining_steps)

		cnx.close()
		sleep(MOTOR_LOOP_RUN_WAIT)  # save some resources



def main():
	# program loop
	while True:
		try:
			GPIOUtility.setup_GPIO()
			GPIOUtility.disable_motor()  # on program start, allow for manual movement

			threads =	[
							Thread(target=primary_process_loop),
							Thread(target=AdafruitFeed.start_client_loop),
							Thread(target=CalibrateCurtain.manual_move_loop),
							Thread(target=DaytimeEvents.sunrise_loop),
							Thread(target=DaytimeEvents.sunset_loop),
							Thread(target=EventPredictor.predictor_loop)
						]
			for thread in threads:
				thread.start()
			for thread in threads:
				thread.join()

		except Exception as error:
			try: ErrorWriter.write_error(error)  # doubly protect main program loop
			except: print(error)
		sleep(ERROR_WAIT)  # something messed up; give it time to reset


if __name__ == '__main__':
	main()
