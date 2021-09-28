# `~/SmartCurtain/3DModels/`

---

## DESCRIPTION
This folder holds the 3D Models for the SmartCurtain.

### CONTENTS
- `./Fusion/` The folder in which the Fusion 360 version of the models are contained.
- `./STL/` The folder in which the STL version of the models are contained.

---

## PARTS

### DESCRIPTION
- [ ] __*EndCap::FreeWheel*__ The part that goes on an end of the rod that holds a toothless pulley to smoothly wrap the belt around the end of the rod.
- [ ] __*EndCap::Motor*__ The part that goes on an end of the rod that holds the motor to drive the belt around the end of the rod.
- [ ] __*Hanger::Ceiling::End*__ The part that screws into the ceiling and connects to the __*Hanger::Rod*__ to hold the curtain to the ceiling.
- [ ] __*Hanger::Ceiling::Interchange*__ The part that the Interchange screws into, which screws into the ceiling.
- [ ] __*Hanger::Rod*__ The part that slides onto the rod and connects to the __*Hanger::Ceiling::End*__ to hold the curtain to the ceiling.
- [ ] __*Housing::Driver/ESP32::Cover*__ The part that screws into the top of the Housing::Driver/ESP32::Panel.
- [ ] __*Housing::Driver/ESP32::Panel*__ The part that contains the ESP32, TB6600 stepper driver & heatsink, Micro USB & Barrel panel mount cables. This is mounted on the wall.
- [ ] __*Interchange*__ The part that holds the two rods together, and has slots for the belts to go through.
- [ ] __*Shuttle::Middle*__ The part that goes on both sides of the curtain ring and moves the curtain along the rod smoothly, and does not connect to any belts.
- [ ] __*Shuttle::Tether*__ The part that is connect to belts and goes on both sides of the curtain ring to move the curtain along the rod smoothly.
- [ ] __*Shuttle::Tether::DrillGuide*__ The part that is used to drill holes into the ends of the belts to allow for screwholes and easy fitting on the __*Shuttle::Tether*__


---

## ASSEMBLY



---

## PART DETAILS

### *EndCap::FreeWheel*

#### Quantity
1

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* Everywhere


### *EndCap::Motor*

#### Quantity
1

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* Touching Bed


### *Hanger::Ceiling::Interchange*

#### Quantity
1

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* None


### *Hanger::Ceiling::End*
TODO: Update when done

#### Quantity
2

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* Touching Bed


### *Hanger::Rod*
TODO: Update when done

#### Quantity
2

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* None
- *Orientation:* So that you are able to insert a 3mm nut
- *Pause Layer:* 65 & insert 3mm nut


### *Housing::Driver/ESP32::Cover*

#### Quantity
1

#### Settings
- *Line Count:* 3
- *Bottom Layers:* 2
- *Top Layers:* 3
- *Infill:* 15%
- *Support:* None


### *Housing::Driver/ESP32::Panel*

#### Quantity
1

#### Settings
- *Line Count:* 3
- *Bottom Layers:* 3
- *Top Layers:* 3
- *Infill:* 20%
- *Support:* Everywhere


### *Interchange*

#### Quantity
1

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* Touching Bed


### *Shuttle::Middle*

#### Quantity
0, 4, 8, 12 (or whatever multiple of 4 you need)

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* Touching Bed


### *Shuttle::Tether*

#### Quantity
4

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* Touching Bed


### *Shuttle::Tether::DrillGuide*

#### Quantity
0 or 1 (if you want to eyeball it, you can)

#### Settings
- *Line Count:* 4
- *Bottom Layers:* 4
- *Top Layers:* 4
- *Infill:* 25%
- *Support:* None
