
# Smart Curtain
This project is not yet complete.

## ABOUT

### DESCRIPTION

Smart Curtain is an automation of curtains to open curtains to a certain position either immediately, at a specified time, at a predicted time, at sunrise/sunset, or from a Google Assistant command.
It is a Hub-Node (master-slave) system.  The Hub runs using a Python(Flask) web server for local network interfacing, a MySQL server for storing curtains, their states and user variables, and a python instance for automatic event creation & management.  The Node runs Arduino compiled and bootloaded code. 

Legacy .stl files for a 3D model can be found on [Thingiverse](https://www.thingiverse.com/thing:3361474)

### FEATURES
- Immediate movement to a specified position
- Move to a position at a specified time
- Event predition
- Sunrise opening and sunset closing
- Google Assistant integration (Adafruit IO via IFTTT)
- Device knows if it has been manually opened

### IMPROVEMENTS
- End stop sensing to prevent curtain from going out of travel length
- Position specific commands—device has knowledge of length of curtain and current position

### CONTENTS
- `~/Hub` The scripts for the Raspberry Pi/Hub system to server requests from user & nodes
- `~/Node` The arduino program to query the Hub for action information.
- `.gitignore` SELF EXPLANATORY
- `.README.md` SELF EXPLANATORY

### HARDWARE
- Raspberry Pi with W/LAN capabilities
- Ethernet enabled Arduino Dev Board *(Teensy 3.2 + WizNet850io Ethernet module)*
- Stepper Motor and Controller *(Nema 17 & TB 6600)*
- 2 End Stop Sensors *(HallEffect)*
- 3D printed parts to make this sucker work

---

## INSTALLATION

### ARDUINO
- Open `~/SmartCurtain/Node/Node.ino` into Arduino IDE (or equivalent).
- Edit `User.h` to match data to Hub IP address & curtain ID.
- Compile & Bootload program to Arduino.

### RASPBERRY PI
- Clone repository to Raspberry Pi.
- Edit `~/SmartCurtain/Hub/DB/Sample.sql` as needed for your setup.
- Edit `~/SmartCurtain/Hub/Python/Other/Global.py` to supply your locale & information.
- `cd` into `~/SmartCurtain/Hub/`.
- Run `make` command.

### HARDWARE SETUP
**Changed; do not trust instructions**

- Make sure hardware works individually (See diagrams below for specifics).
- Connect all devices to network and assemble curtain.

#### MOTOR
```
_
 |_
M  |----- Wire 1
o  |----- Wire 2
t  |
o  |----- Wire 3
r _|----- Wire 4
_|

```

#### DRIVER
```
_______
Ena - °|-------------------
Ena + °|–– R-Pi 13 (GPIO)  |
Dir - °|-------------------+---- R-Pi 6  (GND)
Dir + °|–– R-Pi 7 (GPIO)   |
Pul - °|-------------------
Pul + °|–– R-Pi 11 (GPIO)
B-    °|–– Wire 1
B+    °|–– Wire 2
A-    °|–– Wire 3
A+    °|–– Wire 4
V-    °|–– -
V+    °|–– +
–––––––
```

### Accounts Setup (if using Google Assistant)
- Create accounts for whatever options & feeds desired.
    - Adafruit IO feeds to open & close.
    - IFTTT connection to Google Assistant & Adafruit IO.

---

## NOTES

### INSTALLED EXTERNAL PACKAGES
- mariadb-server
- python3: 
    - adafruit-io
    - astral
    - mysql-connector
    - numpy
    - RPi.GPIO
    - setuptool
    - sklearn

---

## CONSIDERED FUTURE PACKAGES
- [ ] Google Calendar Event Setter
- [ ] Who Is Home (ping Android phones to see if person is on local network) closing/opening

---

created by: MPZinke on 08.20.2018

edited by: MPZinke on 2020.12.28 to actually make it a README instead of a text file.

Remember that you're making this at your own accord and I take no responsibility for any mistakes or problems that may arise.