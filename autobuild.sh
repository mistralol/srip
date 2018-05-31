#!/bin/bash

echo Building

make all check

FILES=`find . -type f |egrep -E "*\.[h|c|cpp|am]"`


for i in `pidof srip` ; do
    #We ignore orc from gstreamer. Cause its always deleted
    lsof -nnP -p $i 2>/dev/null | grep -vE "orcexec" | grep DEL > /dev/null
    #echo $?
    if [ $? -eq 0 ] ; then
        echo Killing $i
        kill $i
    else
        readlink /proc/$i/exe | grep deleted
        if [ $? -eq 0 ] ; then
            echo Killing $i
            gkill -a $i
        fi
    fi
done

inotifywait -e modify $FILES

exec $0
