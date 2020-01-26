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

import RPi.GPIO as GPIO
from time import sleep

from Definitions import *
import DBFunctions
import ErrorWriter
import GPIOUtility


# ——————————————— KNOWN DIRECTION ————————————————

def count_steps_from_closed_to_open():
	count = 0
	while True:
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		count += 1
		if GPIO.input(OPEN_STOP_PIN): return count


def move_to_closed(closed_direction):
	GPIO.output(DIRECTION_PIN, closed_direction)

	while True:
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		if GPIO.input(CLOSED_STOP_PIN): return
		elif GPIO.input(OPEN_STOP_PIN): raise Exception("Direction is not properly set")


# strictly for distance calibration (NOT FOR DIRECTION WHICH MUST BE PRESET)
def calibrate(cnx, cursor):
	try:
		import RPi.GPIO as GPIO
		from DBFunctions import direction, set_curtain_length

		GPIOUtility.enable_motor()
		closed_direction = direction(cursor)
		move_to_closed(closed_direction)
		step_count = count_steps_from_closed_to_open()
		set_curtain_length(cnx, cursor, step_count)  # write count to DB
		GPIOUtility.disable_motor()
	except Exception as error:
		try: GPIOUtility.disable_motor()
		except: pass
		ErrorWriter.write_error(error)


# ——————————————— UNKNOW DIRECTION ———————————————

def count_steps_to_end(opposite_pin):
	GPIO.output(DIRECTION_PIN, False)

	count = 0
	while True:
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		count += 1
		if GPIO.input(OPEN_STOP_PIN): return count


def move_to_an_end():
	GPIO.output(DIRECTION_PIN, True)

	while True:
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		if GPIO.input(CLOSED_STOP_PIN): return CLOSED_STOP_PIN
		elif GPIO.input(OPEN_STOP_PIN): return OPEN_STOP_PIN


def setup(cnx, cursor):
	import RPi.GPIO as GPIO
	from DBFunctions import set_direction_switch, set_curtain_length

	if GPIO.input(OPEN_STOP_PIN) or GPIO.input(CLOSED_STOP_PIN):
		raise Exception("To determine direction, curtain cannot be at an end")

	GPIOUtility.enable_motor()
	# move to end and orient
	first_stop_pin = move_to_an_end()
	# True (pin) & going "negative" (CLOSED_STOP_PIN) is `curtain_direction` = False
	# True (pin) & going "positive" (OPEN_STOP_PIN) is `curtain_direction` = True
	closed_direction = False if first_stop_pin == CLOSED_STOP_PIN else True
	set_direction_switch(cnx, cursor, closed_direction)  # write direction to DB

	# get number of steps
	if OPEN_STOP_PIN == first_stop_pin: opposite_pin = CLOSED_STOP_PIN 
	else: opposite_pin = OPEN_STOP_PIN
	step_count = count_steps_to_end(opposite_pin)
	set_curtain_length(cnx, cursor, step_count)  # write count to DB
	GPIOUtility.disable_motor()


# —————————————— MAIN CALIBRATION —————————————————

def calibrate_if_necessary(cnx, cursor, remaining_steps):
	from DBFunctions import curtain_length

	total_steps = curtain_length(cursor)
	# check if motor stopped prematurely
	leniency = STOPPED_PERCENT_LENIENCY * total_steps / 100
	if leniency < remaining_steps: calibrate(cnx, cursor)


# ——————————————— END CHECKERS —————————————————

# thread to check whether the curtain has manually been moved to either end of curtain
# do not reset position if already marked as at end
def manual_move_loop():
	import RPi.GPIO as GPIO
	from time import sleep
	from GPIOUtility import curtain_is_closed, curtain_is_fully_open, motor_is_engaged
	from DBFunctions import 	connect_to_DB, curtain_length, \
								current_position, set_current_position

	while True:
		cnx, cursor = connect_to_DB()

		if motor_is_engaged():  # motor is currently activated; wait before next check
			sleep(INTERVAL_BETWEEN_MANUAL_MOVEMENT_CHECKS)
			continue

		length = curtain_length(cursor)
		position = current_position(cursor)
		# if curtain open and position not marked at open end
		if curtain_is_fully_open() and length != position:
			DBFunctions.set_current_position(cnx, cursor, length)
		# if curtain closed and position not marked at closed end
		elif curtain_is_closed() and position:
			DBFunctions.set_current_position(cnx, cursor, 0)

		cnx.close()
		sleep(INTERVAL_BETWEEN_MANUAL_MOVEMENT_CHECKS)
