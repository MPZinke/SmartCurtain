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
import sys
import threading
import traceback
from typing import Any, Dict, List
from werkzeug.exceptions import Forbidden, HTTPException, Unauthorized


from Server import Routes
from SmartCurtain import SmartCurtain


def compare_function_params(func1: Dict[str, type], func2: Dict[str, type]) -> Dict[str, type]:
	bad_function_params = {}
	for param in set(list(func1) + list(func2)):
		if(param != "return" and (param not in func1 or param not in func2 or func1[param] != func2[param])):
			bad_function_params[param] = [func[param] if(param in func) else None for func in [func1, func2]]

	return bad_function_params


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
		self.route("/homes/<int:home_id>", Routes.homes.GET_home_id, secure=False)
		self.route("/homes/<int:home_id>/structure", Routes.homes.GET_home_id_structure, secure=False)
		self.route("/homes/<int:home_id>/rooms", Routes.homes.GET_home_id_rooms, secure=False)
		self.route("/homes/<int:home_id>/curtains", Routes.homes.GET_home_id_curtains, secure=False)
		self.route("/homes/<int:home_id>/events", Routes.homes.GET_home_id_events, secure=False)

		self.route("/rooms", Routes.rooms.GET, secure=False)
		self.route("/rooms/<int:room_id>", Routes.rooms.GET_room_id, secure=False)
		self.route("/rooms/<int:room_id>/curtains", Routes.rooms.GET_room_id_curtains, secure=False)
		self.route("/rooms/<int:room_id>/events", Routes.rooms.GET_room_id_events, secure=False)

		self.route("/curtains", Routes.curtains.GET, secure=False)
		self.route("/curtains/<int:curtain_id>", Routes.curtains.GET_curtain_id, secure=False)
		self.route("/curtains/<int:curtain_id>/events", Routes.curtains.GET_curtain_id_events, secure=False)

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

		print(str(error), exception_traceback, file=sys.stderr)
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
	def _params_for_url(url: str) -> Dict[str, type]:
		params = re.findall(r"<(int|string):([_a-zA-Z][_a-zA-Z0-9]*)>", url)
		return {param: {"int": int, "string": str}[type] for type, param in params}


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
		# Ensure all set methods have a callback
		for http_method, callback in methods.items():
			if(not hasattr(callback, '__call__')):
				message = f"""'{http_method}' arg must be of type 'callable', not '{type(callback)}' for URL '{url}'"""
				raise Exception(message)

		url_params = _params_for_url(url)
		for http_method, callback in methods.items():
			callback_params = callback.__annotations__
			bad_params: Dict[str, list[Optional[type]]] = compare_function_params(callback_params, url)
			# Allow only SmartCurtain as a non-specified param; all others will raise an exception.
			if(len(bad_params) > 1 or bad_params.values()[0][0] != SmartCurtain):
				message = f"""'{http_method}' callback '{callback.__name__}' is in compatable with url '{url}'. """
				if(len(callback_missing_params := [param for param in bad_params if(param not in callback_params)])):
					message += f"""'{"', '".join(callback_missing_params)}' missing from '{callback.__name__}'"""

				if(len(url_missing_params := [param for param in bad_params if(param not in url_params)])):
					message += f"""'{"', '".join(url_missing_params)}' missing from '{url.__name__}'"""

				mismatched_types = [param for param, types in bad_params.items() if(types[0] != types[1])]
				raise Exception(f"""'{http_method}' callback is missing arg(s) '{missing_params}' for url '{url}'""")




			if((missing_params := "', '".join([param for param in url_params if(param not in function_args)])) != ""):


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
