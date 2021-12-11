
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

#### Provided Functionality

| Functionality Name | Hardware | Works With Power Cycling | Description |
| --- | --- | --- | --- |
| Auto Calibrate       | Open & Close Endstops | Yes | Curtain can determine its length and update Hub (on movements)  |
| Auto Correct (Close) | Close Endstop         | Yes | Curtain can correct itself moving towards closed position       |
| Auto Correct (Open)  | Open Endstop          | Yes | Curtain can correct itself moving towards open position         |
| Auto Direction       | Open & Close Endstops | Yes | Curtain can determine its direction on startup (with code)      |
| Discrete Location (No Manual Movement) | An Endstop OR Encoder | Yes with Endstop | Move curtain to location relative to an open or closed position. Available between power cycling with an endstop and reset code (travel to endstop counting steps. Travel back, save location, and update hub). If the power cycles with encoder only, the relative location is not known and can cause belt grinding |
| Discrete Location (Manual Movement) | An Endstop AND Encoder | Yes | Move curtain to location relative to an open or closed position. Available between power cycling with an endstop and reset code (travel to endstop counting steps. Travel back, save location, and update hub) |


### IMPROVEMENTS
- End stop sensing to prevent curtain from going out of travel length
- Position specific commands—device has knowledge of length of curtain and current position

### CONTENTS
- `~/Hub/` The scripts for the Raspberry Pi/Hub system to server requests from user & nodes
- `~/Node/` The arduino program to query the Hub for action information.
- `.gitignore` SELF EXPLANATORY
- `README.md` SELF EXPLANATORY

### HARDWARE
- Raspberry Pi with W/LAN capabilities
- Ethernet enabled Arduino Dev Board *(Teensy 3.2 + WizNet850io Ethernet module)*
- Stepper Motor and Controller *(Nema 17 & TB 6600)*
- 2 End Stop Sensors *(HallEffect)*
- 3D printed parts to make this sucker work

---

## INSTALLATION

### ARDUINO
- Install ESP32 board library `https://dl.espressif.com/dl/package_esp32_index.json`
- Install ArduinoJson library.
- Open `~/SmartCurtain/Node/Node.ino` into Arduino IDE (or equivalent).
- Edit `User.h` to match data to Hub IP address & curtain ID.
- Compile & Bootload program to Arduino.

### RASPBERRY PI
- Requires sudo generated sshkey (and the pubkey on github) if the git repo is cloned with ssh.
- Clone repository to Raspberry Pi.
- Edit `~/SmartCurtain/Hub/DB/Sample.sql` as needed for your setup.
- Edit `~/SmartCurtain/Hub/Python/Other/Global.py` to supply your locale & information.
- `cd` into `~/SmartCurtain/Hub/`.
- Run `make` command.

### HARDWARE SETUP
**Changed: do not trust instructions**

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
Ena + °|–– ESP32 18 (GPIO) |
Dir - °|-------------------+---- ESP32 (GND)
Dir + °|–– ESP32 5  (GPIO) |
Pul - °|-------------------
Pul + °|–– ESP32 19 (GPIO)
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

## UPDATE

### HUB

- `cd ~/SmartCurtain/Hub`
- `make update`

---

## NOTES

### INSTALLED EXTERNAL PACKAGES
- mariadb-server
- python3: 
    - adafruit-io
    - astral
    - mysql-connector
    - numpy
    - setuptool
    - sklearn

---

## CONSIDERED FUTURE ADDITIONS
- [ ] 1. Google Calendar Event Setter
- [ ] 2. Thermostat & light level integration *(if it's cold & dark outside and close curtains for better insolation (and vice versa))*
- [ ] 3. Who Is Home (ping Android phones to see if person is on local network) closing/opening
- [x] 4. Update module automatically updates Hub nightly when origin/Production branch is updated.
- [ ] 5. Mobile App
- [ ] 6. WearOS App
- [x] 7. JSON Log format
- [ ] 8. JSON string output for all objects
- [ ] 9. Updater can run bash script (using same principle as DB updates)

---

created by: MPZinke on 08.20.2018

edited by: MPZinke on 2020.12.28 to actually make it a README instead of a text file.

Remember that you're making this at your own accord and I take no responsibility for any mistakes or problems that may arise.

---

## Appendix
- Nema 17 has a step size of 1.8°/step or 200 steps/revolution [Source](https://www.makerguides.com/tb6600-stepper-motor-driver-arduino-tutorial/)
- TB6600 16x Micro stepping equates to ~80 steps/mm (I measured it to be ~80.5 steps/mm)
- [Example for AMT10](https://hackaday.io/project/9914-open-robotics-eurobot/log/34812-amt10-encoder-setup)
- [Example for Dual Core Arduino](https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/)