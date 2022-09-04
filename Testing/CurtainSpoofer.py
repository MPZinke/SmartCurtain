

from flask import Flask, request

app = Flask(__name__);


CURTAIN_POSITION = 0;
CURTAIN_LENGTH = 32000;


@app.route("/")
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


app.run(host="0.0.0.0", port=8082)