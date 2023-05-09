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


import time


CLOSE = False
OPEN = True


DIRECTION = CLOSE
MOTOR_ENABLED = True


def enable_motor():
	MOTOR_ENABLED = True


def disable_motor():
	MOTOR_ENABLED = False


def is_closed():
	return Global.curtain._virtual_physical_position == 0


def pulse():
	if(MOTOR_ENABLED == False):
		raise Exception("Motor is disabled")

	time.sleep(6e-5 * 2)
	if(DIRECTION == CLOSE):
		Global.curtain._virtual_physical_position -= 1
	else:
		Global.curtain._virtual_physical_position += 1

	if(Global.curtain._virtual_physical_position < 0):
		raise Exception("Software failed to prevent curtain from going past endstop")

	if(Global.curtain.length() < Global.curtain._virtual_physical_position):
		raise Exception("Software moved curtain past fully open")


def set_direction(direction: bool):
	# DIRECTION = int(direction + Global.curtain.direction()) & 0b1
	DIRECTION = direction
