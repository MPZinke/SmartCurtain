# Physical Setup

<!-- what it is -->

<!-- required parts, again -->

## Installation
**These instructions are tentative. Do not trust them and make sure that you make the appropriate changes for your hardware.**

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