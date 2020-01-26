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

from Definitions import *


def curtain_is_closed():
	return not GPIO.input(CLOSED_STOP_PIN)


def curtain_is_open():
	return GPIO.input(CLOSED_STOP_PIN)


def curtain_is_fully_open():
	return not GPIO.input(OPEN_STOP_PIN)


#SUGAR: for disabling motor (so that it be manually slid) and return whether it is at the end
def disable_motor():
	GPIO.output(ENABLE_PIN, True)


#SUGAR: for enabling motor (so that motor has control)
def enable_motor():
	GPIO.output(ENABLE_PIN, False)


def motor_is_engaged():
	return GPIO.input(ENABLE_PIN)


# general GPIO & pin setup
def setup_GPIO():
	GPIO.setwarnings(False)  # ignore any important warnings
	GPIO.cleanup()
	GPIO.setmode(GPIO.BOARD)

	for pin in [DIRECTION_PIN, PULSE_PIN, ENABLE_PIN]:
		GPIO.setup(pin, GPIO.OUT)

	for pin in [OPEN_STOP_PIN, CLOSED_STOP_PIN]:
		GPIO.setup(pin, GPIO.IN)
