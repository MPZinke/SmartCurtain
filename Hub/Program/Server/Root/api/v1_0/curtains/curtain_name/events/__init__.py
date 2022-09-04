#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2022.09.03                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


import Server.Root.api.v1_0.curtains.curtain_name.events.all;
import Server.Root.api.v1_0.curtains.curtain_name.events.new;
from System import System;


# `GET /api/v1_0/curtain/<string:curtain_name>/events`
# Lists options available related to curtains' events.
def GET(system: System, curtain_name: str):
	return {
		"GET /api/v1.0/curtain/<string:curtain_name>/events/all": "Lists all events for a curtain.",
		"POST /api/v1.0/curtain/<string:curtain_name>/events/new": "Creates a new curtain's event with the JSON body.",
		"GET /api/v1_0/curtain/<string:curtain_name>/event/<string:event_time>": "Show information for an event for a curtain."
	};