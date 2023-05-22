#!/opt/homebrew/bin/python3
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


import flask
from flask import Flask, jsonify, request
from flask_cors import CORS
import re
import threading
import traceback
from typing import Any, Dict, List
from werkzeug.exceptions import Forbidden, HTTPException, Unauthorized


from Server import Routes
from SmartCurtain import SmartCurtain


class Server:
	def __init__(self, smart_curtain: SmartCurtain):
		self._SmartCurtain: SmartCurtain = smart_curtain

		self._thread = threading.Thread(name="Server", target=self)
		self._app = Flask(__name__)

		self._cors = CORS(self._app)
		self._app.config['CORS_HEADERS'] = 'Content-Type'
		self._app.register_error_handler(Exception, self.handle_error)
		self._app.after_request(Server.after_request)

		self.route("/", Routes.GET, secure=False)
		self.route("/homes", Routes.homes.GET, secure=False)
		self.route("/homes/<int:homes_id>", Routes.homes.GET_homes_id, secure=False)
		# self.route("/homes/<int:home_id>/rooms")
		# self.route("/homes/<int:home_id>/curtains")
		# self.route("/rooms/<int:room_id>")
		# self.route("/rooms/<int:room_id>/curtains")
		# self.route("/curtains")
		# self.route("/curtains/<int:curtain_id>")

		# self.route("/events")
		# self.route("/events/<int:event_id>")

		# self.route("/options")
		# self.route("/options/all")
		# self.route("/options/<int:option_id>")
		# self.route("/options/<string:option_name>")

	# ———————————————————————————————————————————————————— THREAD ———————————————————————————————————————————————————— #

	def __call__(self) -> None:
		"""
		SUMMARY: Adds routes to server & class, and starts the server instance.
		DETAILS: Sets routes using hardcoded routes, functions & HTTP request methods. Calls the Flask::run method.
		"""
		self._app.run(host="0.0.0.0", port=8080)


	def start(self) -> None:
		self._thread.start()


	# ——————————————————————————————————————————————— REQUEST HANDLING ——————————————————————————————————————————————— #

	def debug(self, flag=True) -> None:
		self._app.debug = flag


	def handle_error(self, error):
		"""
		SUMMARY: Handles the return response for any server error that occurs during a request.
		PARAMS:  Takes the error that has occured.
		FROM: https://readthedocs.org/projects/pallet/downloads/pdf/latest/
		 AND: https://stackoverflow.com/a/29332131
		"""
		if isinstance(error, HTTPException):
			return jsonify(error=str(error)), error.code

		try:
			exception_traceback = traceback.format_exc()
		except:
			exception_traceback = "Unknown traceback"

		return jsonify(error=str(error), traceback=exception_traceback), 500

	@staticmethod
	def after_request(response):
		"""
		FROM: https://stackoverflow.com/a/30717205
		"""
		response.headers["Content-Type"] = "application/json"
		response.headers["SmartCurtain-Version"] = "5.0.0"
		return response


	@staticmethod
	def _validate_HTTP_methods(methods: Dict[str, callable]) -> None:
		http_methods = ["CONNECT", "DELETE", "GET", "HEAD", "OPTIONS", "PATCH", "POST", "PUT", "TRACE"]

		# Ensure all methods are correct HTTP methods.
		if((bad_methods := "', '".join([method for method in methods if(method.upper() not in http_methods)])) != ""):
			raise Exception(f"Method(s) '{bad_methods}' not an HTTP method for URL '{url}'")

		if(len(methods) == 0):  # Ensure at least 1 method supplied
			raise Exception(f"At least one HTTP method must be supplied for URL '{url}")


	@staticmethod
	def _validate_method_callbacks(methods: Dict[str, callable], url: str) -> None:
		for method, function in methods.items():  # Ensure all methods have a callback
			if(not hasattr(function, '__call__')):
				message = f"Method '{method}' arg must be of type 'callable', not '{type(function)}' for URL '{url}'"
				raise Exception(message)

		url_params = re.findall(r"<(?:int|string):([_a-zA-Z][_a-zA-Z0-9]*)>", url)
		for method, function in methods.items():
			function_args = function.__code__.co_varnames
			if((missing_params := "', '".join([param for param in url_params if(param not in function_args)])) != ""):
				raise Exception(f"""Method '{method}' callback is missing arg(s) '{missing_params}'""")


	def route(self, url: str, GET: callable=None, *, secure: bool=True, **methods: Dict[str, callable]) -> None:
		methods = {method.upper(): function for method, function in methods.items()}
		def method_function(*args: list[Any], **kwargs: Dict[str, Any]) -> Any:
			if(secure):
				if("Authorization" not in request.headers):
					raise Unauthorized();

				if(self.unauthorized()):
					raise Forbidden();

			return methods[request.method](self._SmartCurtain, *args, **kwargs)

		if(GET is not None):  # Use the GET argument
			if("GET" in [key.upper() for key in methods]):  # Ensure 'GET' is not doubly supplied
				raise Exception(f"Ambiguous supplying of argument 'GET' and keyword argument 'GET' for URL '{url}'")

			methods["GET"] = GET

		Server._validate_HTTP_methods(methods)
		Server._validate_method_callbacks(methods, url)

		# Set URLs for both urls that do and do not end with '/', with the exception of the root URL
		# Get the url without and with the ending '/', then remove the blank urls (ie if the root url is provided)
		urls = set(url for url in [url.rstrip("/"), (f"{url}/" if(url[-1] != "/") else url)] if(url))
		[self._app.add_url_rule(url, url, method_function, methods=list(methods)) for url in urls]
