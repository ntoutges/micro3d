#!/usr/bin/bash

ENTRY="${1:-tester.c}"

if [[ $* == *--watch* ]] then
    # Make + watch using nodemon
    echo "Watching all *.c filepaths"

    nodemon --watch "*" --ext "c" --exec "./make.sh && ./a.out"
else
    # Just build
    gcc -o a.out *.c "./entry/$ENTRY" -Wall -g -lm
fi
