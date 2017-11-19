# maya-smart_extrusion [WIP]

Maya plugin to perform extrusion of a mesh along a curve and control the taper via control circles.  

**Note:** This plugin was originally written in C++. It was then ported to Python for better portability. The C++ source can be found in the `src/cpp` directory.

## How to use it

1. Copy `src/smart_extrusion.py` in one of your Maya plug-in folders
2. Load `src/smart_extrusion.py` plugin from Maya
3. Execute `src/extrude.py` within Maya
4. Select a mesh face and a curve and then launch the `smartExtrude()` function

## To do

* Better creation command
* UI