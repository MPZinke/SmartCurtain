# `~/SmartCurtain/Node/`

---

## DESCRIPTION
This folder is what goes on the Arduino.

### CONTENTS
- `./Node.ino` The main Arduino program file.
- `./Headers/` The header files for the Arduino program.
- `./src/` The .cpp files for the Arduino program.

---

## INSTALLATION
- Open `~/Node/Node.ino` into Arduino IDE (or equivalent).
- Install ESP32 board library `https://dl.espressif.com/dl/package_esp32_index.json`
- Install [ArduinoJson library](https://arduinojson.org/).
- Install [ArduinoMQTTClient library](https://github.com/arduino-libraries/ArduinoMqttClient).
- Edit `./src/Config.cpp.template` adding your values for,
	- The pins as they are attached to the stepper driver.
	- The pulse wait time in coordination to the stepper driver settings.
	- The MQTT broker domain & port.
	- The MAC address of the device as needed.
	- Your WiFi login.
- Save your edited `./src/Config.cpp.template` as `./src/Config.cpp`.
- Compile the project to the ESP32.


## Communication


### Commands
There are 3 basenames that describe a type of action:
- `../move` Command to move a curtain or set of curtains to a position.
- `../status` Command to request the status of a curtain or set of curtains.
	- Publishing to a status topic is the request for the state of the topic's subscriber.
- `../update` Command to update a curtain.
	- Publishing to an update topic is the posting of the state of the topic's publisher.
	- IE. The subscriber's known information will be updated.

### Grouping
The move and status commands can be grouped on a home or room level. The curtains under either the home or room (indicated by the topic) will pickup the commands by subscribing to that topic.

Path formatting is as follows:
`SmartCurtain/all/<command>`
`SmartCurtain/hub/<command>`
`SmartCurtain/<home_id>/<command>`
`SmartCurtain/-/<room_id>/<command>`
`SmartCurtain/-/-/<curtain_id>/<command>`

![image](../Documentation/Images/MQTT/MQTTCommunicationFlowDiagram.jpg)

### Subscribing
The Curtain automatically subscribes to
- `SmartCurtain/all/move`
- `SmartCurtain/all/status`
- `SmartCurtain/-/-/<curtain_id>/move`
- `SmartCurtain/-/-/<curtain_id>/status`
- `SmartCurtain/-/-/<curtain_id>/update`

When the hub updates the curtain, it will then subscribe to
- `SmartCurtain/<home_id>/move`
- `SmartCurtain/<home_id>/status`
- `SmartCurtain/-/<room_id>/move`
- `SmartCurtain/-/<room_id>/status`

The hub and curtain do a shout out to the other when they turn on. This way if the hub is already running and a curtain powers on, the hub will know, and vice versa.
![image](../Documentation/Images/MQTT/MQTTStateDiagram.png)


### `../move`
#### Request
On a move command, a event JSON is sent
```json
{
	"percentage": 100  // (0-100)
}
```

#### Response
Upon starting and completion of a move command, the curtain updates the hub with its status
```json
{
	"id": 1,  // Curtain's ID (1-4294967295)
	"Home.id": 0,  // Curtain's Home's ID (1-4294967295)
	"Room.id": 0,  // Curtain's Room's ID (1-4294967295)
	// ———— HUB CAN SET CURTAIN ———— //
	"Auto Correct": false,  // Whether the Curtain is allowed to Auto Correct
	"length": 32000,  // The allowed length of the Curtain (Sets to minimum of Hub's length & Curtain's length)
	// ———— CURTAIN CAN SET HUB ———— //
	"percentage": 100,  // The current percentage of the Curtain
	"is_moving": true  // Whether the Curtain is moving
}
```


### `../status`
#### Request
No message is sent on a status request

#### Response
On a status command, the curtain updates the hub with its status
```json
{
	"id": 1,  // Curtain's ID (1-4294967295)
	// ———— HUB CAN SET CURTAIN ———— //
	"Home.id": 0,  // Curtain's Home's ID (0-4294967295)
	"Room.id": 0,  // Curtain's Room's ID (0-4294967295)
	"Auto Correct": false,  // Whether the Curtain is allowed to Auto Correct
	"length": 32000,  // The allowed length of the Curtain (Sets to minimum of Hub's length & Curtain's length)
	// ———— CURTAIN CAN SET HUB ———— //
	"percentage": 100,  // The current percentage of the Curtain
	"is_moving": true  // Whether the Curtain is moving
}
```


### `../update`
#### Request
On an update request, the known information is received and updated where allowed if it is does not match
```json
{
	"id": 1,  // Curtain's ID (1-4294967295)
	// ———— HUB CAN SET CURTAIN ———— //
	"Home.id": 1,  // Curtain's Home's ID (1-4294967295)
	"Room.id": 1,  // Curtain's Room's ID (1-4294967295)
	"Auto Correct": false,  // Whether the Curtain is allowed to Auto Correct
	"length": 32000,  // The allowed length of the Curtain (Sets to minimum of Hub's length & Curtain's length)
	// ———— CURTAIN CAN SET HUB ———— //
	"percentage": 0,  // The current percentage of the Curtain
	"is_moving": false  // Whether the Curtain is moving
}
```

#### Response
On an update command, the curtain updates the hub with its update
```json
{
	"id": 1,  // Curtain's ID (1-4294967295)
	// ———— HUB CAN SET CURTAIN ———— //
	"Home.id": 1,  // Curtain's Home's ID (0-4294967295)
	"Room.id": 1,  // Curtain's Room's ID (0-4294967295)
	"Auto Correct": false,  // Whether the Curtain is allowed to Auto Correct
	"length": 32000,  // The allowed length of the Curtain (Sets to minimum of Hub's length & Curtain's length)
	// ———— CURTAIN CAN SET HUB ———— //
	"percentage": 100,  // The current percentage of the Curtain
	"is_moving": true  // Whether the Curtain is moving
}
```


## Software
![image](../Documentation/Images/Software/ADS_Node.jpg)


## Hardware
![image](../Documentation/Images/Hardware/MQTTStateDiagram.png)
