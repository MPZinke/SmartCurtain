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
import ErrorWriter
import GPIOUtility


# —————————————————— UTILITY ——————————————————–

def curtain_final_position(desired_location, remaining_steps, stop_pin, total_steps):
	if not remaining_steps: return desired_location  # success
	return total_steps if stop_pin == OPEN_STOP_PIN else 0  # open or closed position


def needed_steps(cursor, new_position, total_steps):
	from DBFunctions import current_position

	current_step_position = current_position(cursor)
	if new_position > current_step_position: return current_step_position
	elif new_position < 0: return 0
	return new_position - current_step_position


# —————————————————— MOTOR ——————————————————–

# send a pulse to motor, then check whether the curtain has reached fully open/close
def step_motor_to_location_or_to_end(opposing_stop_pin, steps_to_move):
	while 0 < steps_to_move:
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		steps_to_move -= 3
		if not GPIO.input(opposing_stop_pin): return steps_to_move  # check reached end of span

	return 0


# ——————————————— RUNNING FUNCTIONS ———————————————

# main activation function
def move_curtain(direction, steps_to_move, stop_pin):
	# run motor
	GPIOUtility.enable_motor()  # VERY NECESSARY: lets driver have control
	GPIO.output(DIRECTION_PIN, direction)
	end_count = step_motor_to_location_or_to_end(stop_pin, abs(steps_to_move))

	GPIOUtility.disable_motor()  # left without clean up to keep motor disabled
	return end_count  # return final number of steps not taken


def close_curtain(direction):
	# run motor
	GPIOUtility.enable_motor()  # VERY NECESSARY: lets driver have control
	GPIO.output(DIRECTION_PIN, direction)

	while True:
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		if not GPIO.input(CLOSED_STOP_PIN): break

	GPIOUtility.disable_motor()


def open_curtain(direction):
	# run motor
	GPIOUtility.enable_motor()  # VERY NECESSARY: lets driver have control
	GPIO.output(DIRECTION_PIN, direction)

	while True:
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, True)
		sleep(PULSE_WAIT)
		GPIO.output(PULSE_PIN, False)
		sleep(PULSE_WAIT)
		if not GPIO.input(OPEN_STOP_PIN): break

	GPIOUtility.disable_motor()


# check DB to orient the motor direction, calculate steps & move curtain
def activate_curtain(cnx, cursor, new_position_steps):
	from DBFunctions import curtain_length, direction, new_position

	total_steps = curtain_length(cursor)
	if not total_steps: CalibrateCurtain.calibrate(cnx, cursor)  # check if curtain length not set up

	if total_steps - 5 <= new_position_steps:
		open_curtain(False ^ direction(cursor))
		return new_position(cnx, cursor, total_steps)
	elif new_position_steps <= 5:
		close_curtain(True ^ direction(cursor))
		return new_position(cnx, cursor, 0)

	# calculate steps needed to take from current position to desired
	steps_to_move = needed_steps(cursor, new_position_steps, total_steps)

	# move curtain to position: direction based on DB switch
	move_direction = (steps_to_move < 0) ^ direction(cursor)
	# pin of stop switch, into which curtain can run
	# position is approaching 0 (closed) if negative steps; else increasing position (open)
	stop_pin = CLOSED_STOP_PIN if steps_to_move < 0 else OPEN_STOP_PIN
	remaining_steps = move_curtain(move_direction, steps_to_move, stop_pin)

	final_position = curtain_final_position(new_position_steps, remaining_steps, stop_pin, total_steps)
	new_position(cnx, cursor, final_position)

	return remaining_steps  # returned to see whether it failed
