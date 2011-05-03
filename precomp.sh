#/bin/sh
FILES="MainFrame.h Application.h GLCanvas.h EditLocationsDialog.h"

rm *.gch
echo ${FILES} | xargs -L1 g++ `/usr/local/bin/wx-config --cppflags` -c -g -I/usr/local/include
