
# Smart Curtain
This project is not yet complete.

## About
### Features
- Immediate movement to a specified position
- Move to a position at a specified time
- Event predition
- Sunrise opening and sunset closing
- Google Assistant integration
- Device knows if it has been manually opened

### Improvements
- End stop sensing to prevent curtain from going out of travel length
- Position specific commands—device has knowledge of length of curtain and current position

### Hardware
- Raspberry Pi with W/LAN capabilities  (Raspberry Pi 3B+)
- ESP-32 Arduino Dev Board
- Stepper Motor and Controller  (Nema 17 & TB 6600)
- 2 End Stop Sensors


Smart Curtain is an automation of curtains to open curtains to a certain position either immediately, at a specified time, at a predicted time, at sunrise/sunset, or from a Google Assistant command.
It a master-slave system.  The master runs using a PHP server for local network interfacing, a MySQL server for storing curtains, their states and user variables, and a python instance for automatic event creation.  The slave runs on Arduino compiles and bootloaded code. 




Complete .stl files can be found at https://www.thingiverse.com/thing:3361474

–––––––––––––––About–––––––––––––
This is a design for an automated curtain that can link in to Google Assistant.  I have implemented
the design and it works as I have done it.Feel free to change what ever you want about it and give 
me constructive feedback.Currently, there are software designs, a lite version and a more capable
version.  The light version stores its info in a textfile and is a simple client to Adafruit IO
which connects with IFTTT to Google Assistant.  The other version runs a database which connects
to a PHP localhost portal through which you can see the state, schedule events, and view/delete set
events. For the more capable portal version, I would suggest a Rasbperry Pi A+.
TL;DR Lite version you tell Google to open/close your curtains.  Portal version you can do what the
lite version does and set times to open/close. 



–––––––––––––PI Setup––––––––––––
• Install Raspbian by flashing it to a MicroSD card
	 I chose Raspbian Stretch Lite because it's less resource intensive but it can be harder to know what you're doing
• Go to raspi-config
	>> sudo raspi-config
	 change the keyboard to your country under Localization
	 Connect to wifi
	 Allow for SSH under Interfacing
• Copy files to flashdrive and mount it by
	>> mkdir /home/pi/tmp
	>> sudo mount /dev/sda1 /home/pi/tmp


–––––––––––Installation––––––––––
• Start installation script after entering the mounted folder with
	>> cd /home/pi/tmp
	>> bash execute
• Installation will take upwards of an hour because there are a lot of dependencies that need to be installed.  It starts with
	the general Rasbian update/upgrade. See the full list below:
• During this process most of it is automatic, but at the beginning you will be prompted for the password you use when logging
	in.  This is necessary and used to access the database once it is created.
• Then you can optionally enter a static IP address for the Pi to stay on.  This will be useful for when accessing the web-
	portal.



––––––––Necessary Hardware–––––––
-4 3mm screws (~12mm)
-6 2.5mm screws (~5mm)
-4 2mm screws (~4mm)
-2 #6 machine screws and matching nuts (~2")
-1 #6 machine screw and matching nuts (~1.5")
-18 wall screws
-Raspi A+ or Zero W (I perfer to support Adafruit for these)
–Raspi power supply
-~8Gb Micro SD card for Raspi Zero
-Temporarily need connector for HDMI mini, micro USB
-Cabel to go from wall mount to wall mount, back and a little more
-2 cable clamps to match guage of cable
-2 cable ferrules (or two more clamps) for cable
-Nema 17 Motor and TB6600 Driver https://smile.amazon.com/gp/product/B074TCM8ZL/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1
-Timing belt and grooved pulley for motor https://smile.amazon.com/dp/B01N5VDSV2/ref=cm_sw_em_r_mt_dp_U_qq-qCbE09RPSS
-Grooved ball bearing https://smile.amazon.com/gp/product/B00YBXOMRE/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1
-Power adapter female to screw terminal connector https://smile.amazon.com/gp/product/B00ZGDF7AY/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1
-12V 2A (or 3A) 2.1mm power adapter
-22GA (or thicker) insullated wire
-Other wire to connect motor drive to Raspberry Pi
-3D printed parts



––––––––––Hardware Setup–––––––––
This is how I would suggest setting it up.  First make sure everything is working, then attach everything to the wall.
• Once the housings are screwed into the wall, put the screws for the motor into their holes.
• Screw on the Raspi
• Place the stepper driver into the driver case and screw in screws from bottom
• Attach wires and cables to power devices, though I would suggest running and testing in console before moving to hardware


_
 |_
M  |----- Wire 1
o  |----- Wire 2
t  |
o  |----- Wire 3
r _|----- Wire 4
_|

Driver:
_______
Ena - °|______________
Ena + °|–– π pin 13   |
Dir - °|––––––––––––––+------  to π pin 6 GND
Dir + °|–– π pin 7    |
Pul - °|______________|
Pul + °|–– pin 11
B-    °|–– Wire 1
B+    °|–– Wire 2
A-    °|–– Wire 3
A+    °|–– Wire 4
V-    °|–– -
V+    °|–– +
–––––––


––––––––––Account Setup––––––––––
• Create an adafruit IO account at https://accounts.adafruit.com/users/sign_up
• In it create 2 feeds: one for each of opening curtain, closing curtain
• In IFTTT create applets for each of the Adafruit feeds.  If you have any questions feel free to ask me


––––––––––Setup Software–––––––––
• Installed Packages:
	 php
	 mysql-server
	 php-mysql
	 libatlas-base-dev
• Installed python wheels
	 adafruit-io
	 RPi.GPIO
	 numpy
	 sklearn


––––––––––Running Script–––––––––
INCLUDED SCRIPTS ARE:
• Adafruit IO Setup
• Change Direction The Motor Spins 
	 used to change the direction the motor turns for each command (eg "Google open the curtain" and they close)
	 once inside ~/SmartCurtain/ activate with
	>> bash ChangeDirection
• Change Distance The Curtain Opens
• Configure Static IP Address
• Turn On/Off Future Event Prediction
• Switch State

created by: MPZinke on 08.20.2018
Remember that you're making this at your own accord and I take no responsibility for any mistakes or problems that may arise