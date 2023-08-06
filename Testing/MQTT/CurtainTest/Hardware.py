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


import HardwareGlobal


CLOSE = False
OPEN = True


def enable_motor():
	MOTOR_ENABLED = True


def disable_motor():
	MOTOR_ENABLED = False


def is_closed():
	import Global

	return Global.curtain._virtual_physical_position == 0


def pulse():
	import Global

	if(HardwareGlobal.MOTOR_ENABLED == False):
		raise Exception("Motor is disabled")

	time.sleep(6e-5 * 2)
	if(HardwareGlobal.DIRECTION == CLOSE):
		Global.curtain._virtual_physical_position -= 1
	else:
		Global.curtain._virtual_physical_position += 1

	if(Global.curtain._virtual_physical_position < 0):
		raise Exception("Software failed to prevent curtain from going past endstop")

	if(Global.curtain.length() < Global.curtain._virtual_physical_position):
		raise Exception(f"Software moved curtain past fully open at {Global.curtain._virtual_physical_position}")


def set_direction(direction: bool):
	# DIRECTION = int(direction + Global.curtain.direction()) & 0b1
	HardwareGlobal.DIRECTION = direction
