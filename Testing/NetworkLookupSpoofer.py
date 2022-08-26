

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


app.run(host="0.0.0.0", port=8081)