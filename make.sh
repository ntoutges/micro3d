#!/usr/bin/bash

ENTRY="${1:-tester.c}"

ENTRIES="./entry/$ENTRY"

# Include mongoose if required
if [ "$ENTRY" == "socket.c" ]; then
    ENTRIES="$ENTRIES ./mongoose/mongoose.c"
fi

if [ "$2" == "--watch" ]; then
    # Make + watch using nodemon
    echo "Watching all *.c filepaths"

    nodemon --watch "*,entry/*" --ext "c" --exec "./make.sh $ENTRY && ./a.exe" --legacy-watch
else
    # Just build
    gcc -o a.exe src/*.c src/cmd/*.c $ENTRIES -Wall -g -lm -Os
fi
