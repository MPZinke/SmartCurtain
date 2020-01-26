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

# —————————————— USER SPECIFIC EVENTS ——————————————

CITY = "Dallas"
USER_FEED_KEY = "7aad007f269d46219ac27a4b602571ce"
USER_FEED_NAME = "MPZinke"
OPEN_FEED = ""
CLOSE_FEED = ""


# ——————————————— DAYTIME EVENTS ————————————————

SAME_EVENT_TIME_DIFFERENCE = 20


# ————————————————— DATABASE ——————————————————

DB_NAME = "curtain"
DB_USER = "python_db_user"
DB_PASSWD = ""
DB_IP = "localhost"
DB_PORT = 3306


# ————————————————— ERROR LOG —————————————————

DELIMITER = '|'  # how values are sepparated in logs
ERROR_FILE_NAME = "ErrorLog.txt"


# ——————————————— EVENT PREDICTION ———————————————

CLUSTER_TIME_SPREAD = 1  # multiply by 2 for total diameter


# —————————————————— RPI.GPIO ——————————————————

DIRECTION_PIN = 7
PULSE_PIN = 11
ENABLE_PIN = 13

OPEN_STOP_PIN = 16
CLOSED_STOP_PIN = 18

PULSE_WAIT = .0000001


# —————————————————— SLEEP ———————————————————

ERROR_WAIT = 5
EVENT_PREDICTOR_CHECK_SLEEP = 60
EVENT_PREDICTOR_SLEEP = 86340
FEED_CLIENT_CHECK_LOOP = 20
INTERVAL_BETWEEN_MANUAL_MOVEMENT_CHECKS = 10
MOTOR_LOOP_RUN_WAIT = 1
SUNRISE_LOOP_WAIT = 60
SUNSET_LOOP_WAIT = 60


# ——————————————————— TIME ———————————————————

DATETIME_STRING_FORMAT = "%Y-%m-%d %H:%M:%S"
CLUSTER_SPAN = 4  # number of weeks looked back on to determine applicable logs


# —————————————————— OTHER ———————————————————

STOPPED_PERCENT_LENIENCY = 5  # percentage of remaining steps before



#SUGAR: used in if statements: check if value null; if null, sleep; return truthiness
def is_null_sleep_then(evalutated_value, sleep_amount=5):
	if not evalutated_value: sleep(sleep_amount)
	return not bool(evalutated_value)
