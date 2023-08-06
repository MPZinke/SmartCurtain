#!/opt/homebrew/bin/python3
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
import sys


app = Flask(__name__);


PORT = int(sys.argv[1]) if(len(sys.argv) > 1) else 8081;

CURTAIN_POSITION = 0;
CURTAIN_LENGTH = 32000;


@app.route("/api/v1.0/network/label/Home/services/label/SmartCurtain")
def curtain_services():
	return json.dumps(
	[
	    {
	        "id": 1,
	        "auth_value": "HelloWorld",
	        "label": "SmartCurtain",
	        "port": 8082,
	        "device": {
	            "id": 5,
	            "address": "host.docker.internal",
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
	                "gateway": "192.168.1.1",
	                "netmask": "255.255.255.0"
	            }
	        }
	    },
	    {
	        "id": 2,
	        "auth_value": "Hello World2",
	        "label": "SmartCurtain",
	        "port": 8083,
	        "device": {
	            "id": 9,
	            "address": "host.docker.internal",
	            "label": "Bedroom-Curtain",
	            "is_reservation": False,
	            "is_static": True,
	            "mac": "00:00:00:00:00:02",
	            "groups": [
	                {
	                    "id": 3,
	                    "label": "Bedroom"
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
	                "gateway": "192.168.1.1",
	                "netmask": "255.255.255.0"
	            }
	        }
	    }
	])


app.run(host="0.0.0.0", port=PORT)
