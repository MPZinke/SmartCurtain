

from flask import Flask

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
	return [
	    {
	        "address": "192.168.1.6",
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
	    {
	        "address": "10.0.0.10",
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
	            "gateway": "",
	            "netmask": "255.255.255.0"
	        }
	    }
	]


app.run(host="0.0.0.0", port=8081)