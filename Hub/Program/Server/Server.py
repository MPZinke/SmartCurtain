#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2020.12.23                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


from flask import Flask, request, session;
from pathlib import Path;
import sys;
from time import sleep;
from typing import List;


from Server import Route;
from System import System;
from Utility import try_convert;
import Utility.Logger as Logger;
from Utility.ZThread import ZWidget;


class Server(ZWidget):
	def __init__(self, System):
		ZWidget.__init__(self, "Server", System);

		self._app = Flask(__name__);
		self.add(
		  [
			Route("/api/index"),
			Route("/api/v1.0/index"),
			# Route("/api/v1.0/curtains"),
			# Route("/api/v1.0/curtains/all"),
			# Route("/api/v1.0/curtains/new", "POST"),
			# Route("/api/v1.0/curtain/<int:curtain_id>", "GET", "PATCH"),
			# Route("/api/v1.0/curtain/<string:curtain_name>", "GET", "PATCH"),
			# Route("/api/v1.0/curtain/<string:curtain_name>/events"),
			# Route("/api/v1.0/curtain/<int:curtain_id>/events"),
			# Route("/api/v1.0/curtain/<string:curtain_name>/events/all"),
			# Route("/api/v1.0/curtain/<string:curtain_name>/events/new", "POST"),
			# Route("/api/v1.0/curtain/<int:curtain_id>/events/all"),
			# Route("/api/v1.0/curtain/<int:curtain_id>/events/new", "POST"),
			# Route("/api/v1.0/curtain/<string:curtain_name>/event/<string:event_time>", "GET", "PATCH"),
			# Route("/api/v1.0/curtain/<string:curtain_name>/event/<int:event_id>", "GET", "PATCH"),
			# Route("/api/v1.0/curtain/<int:curtain_id>/event/<string:event_time>", "GET", "PATCH"),
			# Route("/api/v1.0/curtain/<int:curtain_id>/event/<int:event_id>", "GET", "PATCH"),
			# Route("/api/v1.0/events"),
			# Route("/api/v1.0/events/all"),
			# Route("/api/v1.0/event/<int:event_id>"),
			# Route("/api/v1.0/event/new", "POST"),
			# Route("/api/v1.0/options"),
			# Route("/api/v1.0/options/all"),
			# Route("/api/v1.0/option/<int:option_id>"),
			# Route("/api/v1.0/option/<string:option_name>")
		  ]
		);


	def add(self, routes: List[Route]) -> None:
		for route in routes:
			route.add_to_server(self._app, self._System);


	def debug(self, flag=True) -> None:
		self._app.debug = flag;


	def _loop_process(self) -> None:
		"""
		SUMMARY: Adds routes to server & class, and starts the server instance.
		DETAILS: Sets routes using hardcoded routes, functions & HTTP request methods. Calls the Flask::run method.
		"""
		self._app.run(host="0.0.0.0", port=80);
