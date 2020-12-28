# `~/SmartCurtain/Node/`

---

## DESCRIPTION

This folder contains what goes on the Arduino.  It contains:
- `Curtain.h` Main scope of Curtain object as what the Arduino needs to know.  Includes activation checks, state info, etc.
- `Global.h` Information and variables that are shared across the program.
- `GPIO.h` The hardware activation functions.
- `Node.ino` The main file that is uploaded and uses uses other header files as dependencies.
- `Transmission.h` Functions that send/receive and encode/decode HTTP requests.
- `User.h` User and device specific information.
