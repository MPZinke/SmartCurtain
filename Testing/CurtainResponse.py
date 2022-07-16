

import json
import requests
from time import sleep


def pretty_print_POST(request):
	"""
	At this point it is completely built and ready
	to be fired; it is "prepared".

	However pay attention at the formatting used in
	this function because it is programmed to be pretty
	printed and may differ from the actual request.
	"""
	print("-----------START-----------")
	print(f"{request.method} {request.url}")
	print("\n".join(f"{k}: {v}" for k, v in request.headers.items()))
	print(request.body)



def request(data: dict) -> None:
	print(json.dumps(data))
	try:
		response = requests.get("http://10.0.0.6", json=data, timeout=16)
		print(response.status_code)
		print(response.text)
		print(response.json())
	except Exception as error:
		print(error)

	print()


request({"query type": "status"})
request({"query type": "move", "event": {"id": 4294967295, "percentage": 100}})
request({"query type": "move"})
request({"query type": "reset", "event": {"id": 4294967295, "percentage": 100}})
request({"query type": "reset"})
request({"query type": "wrong"})
request({"wrong": "wrong"})
