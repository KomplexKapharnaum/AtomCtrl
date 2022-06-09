# AtomCtrl

## INSTALL
Edit M5Atom/src/main.cpp to change WIFI settings
Use VSCode + PlatformIO to open M5Atom folder as root folder and flash into Atom / AtomLite over USB.

## USAGE
By default, when onboard button is clicked, Atom will broadcast /go via OSC on UDP port 3333.

Every seconds, it will also look for a Bridge using OSC port 3334.
Once a bridge is detected, Atom will now send /go via OSC on 3334 to the bridge only, and stop broadcast on 3333.
If MQTT is available on the Bridge machine Atom and the Bridge will switch to MQTT to communicate instead of OSC.

### LED
RED: wifi not connected
YELLOW: sending via OSC (broadcast on 3333 or unicast on 3334 if Bridge detected)
GREEN: sending via MQTT to bridge
BLUE: message sent

