

from flask import Flask, request
import sys


app = Flask(__name__);


PORT = int(sys.argv[1]) if(len(sys.argv) > 1) else 8081;

CURTAIN_POSITION = 0;
CURTAIN_LENGTH = 32000;


@app.route("/", methods=["GET", "POST"])
def index() -> str:
	body = request.json;

	if(body.get("query type") == "status"):
		return {
			"id": PORT - 8080,
			"auto calibrate": False,
			"auto correct": True,
			"percentage": int(CURTAIN_POSITION / CURTAIN_LENGTH),
			"position": CURTAIN_POSITION,
			"direction": False,
			"discrete movement": False,
			"length": CURTAIN_LENGTH
		};

	if(body.get("query type") == "move"):
		return {"success":"Moving to position"}

	print(body)
	return {"error": "Your Test Failed"}


app.run(host="0.0.0.0", port=PORT)
