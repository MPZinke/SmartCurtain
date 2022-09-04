#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2022.09.04                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


from flask import Flask, request
import json


app = Flask(__name__);


@app.route("/api/v1.0/network/label/Home/device/label/Livingroom-Curtain")
def livingroom_curtain() -> str:
	print("livingroom_curtain")
	return {"address": "10.0.0.13"}


@app.route("/api/v1.0/network/label/Home/device/label/Bedroom-Curtain")
def bedroom_curtain() -> str:
	print("livingroom_curtain")
	return {"address": "10.0.0.14"}


@app.route("/api/v1.0/network/label/Home/devices/group/label/Curtain")
def all_curtains() -> str:
	return json.dumps([
		    {
		        "address": "localhost:8081",
		        "label": "Livingroom-Curtain",
		        "is_reservation": False,
		        "is_static": True,
		        "mac": "00:00:00:00:00:01",
		        "groups": [
		            {
		                "id": 2,
		                "label": "Livingroom"
		            },
		            {
		                "id": 10,
		                "label": "Smart"
		            },
		            {
		                "id": 11,
		                "label": "Curtain"
		            }
		        ],
		        "Network": {
		            "id": 1,
		            "auth_value": "",
		            "label": "Home",
		            "gateway": "",
		            "netmask": "255.255.255.0"
		        }
		    },
		    # {
		    #     "address": "localhost:8083",
		    #     "label": "Bedroom-Curtain",
		    #     "is_reservation": False,
		    #     "is_static": True,
		    #     "mac": "00:00:00:00:00:02",
		    #     "groups": [
		    #         {
		    #             "id": 3,
		    #             "label": "Bedroom"
		    #         },
		    #         {
		    #             "id": 10,
		    #             "label": "Smart"
		    #         },
		    #         {
		    #             "id": 11,
		    #             "label": "Curtain"
		    #         }
		    #     ],
		    #     "Network": {
		    #         "id": 1,
		    #         "auth_value": "",
		    #         "label": "Home",
		    #         "gateway": "",
		    #         "netmask": "255.255.255.0"
		    #     }
		    # }
		])


CURTAIN_POSITION = 0;
CURTAIN_LENGTH = 32000;


@app.route("/", methods=["GET", "POST"])
def index() -> str:
	body = request.json;

	if(body.get("query type") == "status"):
		return {
			"id": 1,
			"auto calibrate": False,
			"auto correct": True,
			"percentage": int(CURTAIN_POSITION / CURTAIN_LENGTH),
			"position": CURTAIN_POSITION,
			"direction": False,
			"discrete movement": False,
			"length": CURTAIN_LENGTH
		};

	if(body.get("query type") == "move"):
		print("here")
		return {"success":"Moving to position"}

	print(body)
	return {"error": "Your Test Failed"}


app.run(host="0.0.0.0", port=8081)