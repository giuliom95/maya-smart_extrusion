OUT="smart_extrusion"
STD="c++11"
MAYA_PATH="/usr/autodesk/maya2017"

g++ -std=$STD -c -I$MAYA_PATH/include -fPIC *.cpp
g++ -std=$STD -shared -o $OUT.so *.o -L$MAYA_PATH/lib -lOpenMaya
rm *.o
