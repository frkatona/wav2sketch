# wav file converter for use in Teensy audio applications

Not fully tested yet

Based on Paul Stroffregen's C tool (https://github.com/PaulStoffregen/Audio/blob/master/extras/wav2sketch/wav2sketch.c) as referenced in the hackaday workshop PDF (https://cdn.hackaday.io/files/8292354764928/workshop.pdf) but here written in Python and with some included drum samples.

I needed them for the Teensy 4.1 drum trigger project as copied from the Gadget Reboot youtube channel (https://youtu.be/y2Lmbts9IIs).

## usage

Takes 16-bit wav files and converts them to C++ arrays (.cpp) and header files (.h) for use in Teensy audio applications.  The file structure and code references can be seen on Gadget Reboot's github page (https://github.com/GadgetReboot/Piezo_Interface)