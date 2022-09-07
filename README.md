# Beremiz4Pico
Port of Beremiz PLC to Raspberry Pi Pico

This project was written using Visual Studio 2012 Express Edition, but it can be normally compiled with any more recent VS versions (using Community editions)
It requires wxWidgets framework to be installed and compiled on the machine before it can be compiled. You will probably need to change the paths to wxWidgets
in the VS project file (.vcxproj) as Visual Studio does not use relative paths for them.

Note that Beremiz4Pico will be replaced soon by Beremiz Project Manager project, which is based on plugins, and allows to declare any targets (there is a 
Pico/RP2040 plugin in Beremiz Project Manager, which uses the same source code as Beremiz4Pico)
So, for now, I don't spend more time on Beremiz4Pico as Beremiz Project Manager will replace it.
