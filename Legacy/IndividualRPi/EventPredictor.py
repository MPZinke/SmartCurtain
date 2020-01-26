#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

###################################################
#
#	-Service that loops to activate curtains after 
#	 a scheduled event has passed
#	-Creates future events based on passed events
#
###################################################

from datetime import datetime, timedelta
from numpy import array, where
from sklearn.cluster import KMeans
from time import sleep

from Definitions import *


""" object for each centiod """
class Centoid:
	def __init__(self, start_center, possessed_times):
		self.center = start_center
		self.avg = start_center  # used to record average of most common
		self.possessed_times = possessed_times
		self.valid_logs = []
		self.is_valid = self.check_validity() # is either average of usable points or False


	""" check if three or more points are within eachother
		 if they are, returns the average of points """
	def check_validity(self):
		while True:
			surrounding_times = self.times_surrounding_average()
			avg_of_surroundings = sum(surrounding_times) / len(surrounding_times)
			if avg_of_surroundings == self.avg: break
			self.avg = avg_of_surroundings
		if len(surrounding_times) > 2: return True
		return False


	def times_surrounding_average(self):
		return [time for time in self.possessed_times if((self.avg - CLUSTER_TIME_SPREAD) < time
											and time < (self.avg + CLUSTER_TIME_SPREAD))]


def possible_centroid_count(events):
	number_of_days = len({(time.day,time.month,time.year) for (time) in events})
	number_of_events = events.rowcount
	try: return int(number_of_events / number_of_days)  # 0/0=0 does not work
	except: return None


# ——————————————— TIME CONVERSION ————————————————

# create a clock readout hh:mm:ss for DB storage
def convert_decimal_value_of_day_to_time(times):
	return [':'.join([format(int(time), "02"), str((time-int(time))*60)[:2], "00"]) for time in times]


# convert decimal time to a datetime object
def covert_time_datetime(day, times):
	return [datetime.strptime(day.date().strftime("%Y-%m-%d ")+time, DATETIME_STRING_FORMAT) for time in times]


# decimal value of time for mathmatical calculation of centroid
def convert_time_to_decimal_value_of_day(events):
	return [round(time.hour + time.minute / 60, 3) for time in events]


# —————————————— EVENT CALCULATION ————————————————

def create_centroids(decimal_times, k_value):
	centroids = []
	times = array(decimal_times)
	centers = KMeans(n_clusters=k_value, random_state=0)
	try: centers.fit(times.reshape(-1,1))
	except: return
	for i in range(centers.n_clusters):
		combined = times[where(centers.labels_ == i)]
		centroids.append(Centoid(float(centers.cluster_centers_[i]), combined))
	return centroids


# use kmeans to calculate possible future events for logged times in time frame
def schedule_future_events():
		# calcuate up through the next 7 days
		for x in range(1, 8):
			day = (datetime.now() + timedelta(days=x))  # formated to mysql (Monday:1, Tuesday:2,...Sunday:7)
			previous_events = events_for_day_of_week(cnx, day.weekday(), state)

			# now the math starts
			if not previous_events: continue
			decimal_times = convert_time_to_decimal_value_of_day(previous_events)
			k_value = possible_centroid_count(previous_events)
			centroids = create_centroids(decimal_times, k_value)
			centroid_avgs = [centroid.avg for centroid in centroids]
			future_times = convert_decimal_value_of_day_to_time(centroid_avgs)
			future_events = covert_time_datetime(day, future_times)
			
			for event in future_events: set_future_event(cnx, cursor, state, time)


def predictor_loop():
	from DBFunctions import connect_to_DB, event_predictor

	while True:
		cnx, cursor = connect_to_DB()

		if event_predictor(cursor):
			schedule_future_events()
			sleep(EVENT_PREDICTOR_SLEEP)

		cnx.close()
		sleep(EVENT_PREDICTOR_CHECK_SLEEP)
