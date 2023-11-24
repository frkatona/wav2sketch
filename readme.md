# wav file converter for use in Teensy audio applications

Not fully tested yet

Based on Paul Stroffregen's C tool (https://github.com/PaulStoffregen/Audio/blob/master/extras/wav2sketch/wav2sketch.c) as referenced in the hackaday workshop PDF (https://cdn.hackaday.io/files/8292354764928/workshop.pdf) but here written in Python and with some included drum samples

## usage

I needed them for the Teensy 4.1 drum trigger project as copied from the Gadget Reboot youtube channel (https://youtu.be/y2Lmbts9IIs)

Takes 16-bit wav files and converts them to C++ arrays (.cpp) and header files (.h) for use in Teensy audio applications.  The file structure and code references can be seen on Gadget Reboot's github page (https://github.com/GadgetReboot/Piezo_Interface)

## dev log / to-do

- conversion
  - previous comments suggested 16 bit wav files were necessary, but the example .cpp files seem 32 bit.  Unclear if any version of this is necessary or even where to ballpark the memory constraints, but I'd like to eventually modify this to take at most 16 bit if not mp3 or flac
  - naming convention seems important for the .cpp files, but I'm not sure how to handle this yet

- drum module
  - multi-sample support
    - velocity selection
    - random sample selection
  - fx (reverb, delay, etc...I wonder what the least expensive way to do this is)
  - hardware controls/interface
    - metronome, lighting, gain, fx, sample select, screen, etc.