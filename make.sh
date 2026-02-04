#!/usr/bin/bash

ENTRY="${1:-tester.c}"

# if [[ $* == *--watch* ]] then
#     # Make + watch using nodemon
#     echo "Watching all *.c filepaths"

#     nodemon --watch "*" --ext "c" --exec "./make.sh && ./a.exe"
# else
    # Just build
    gcc -o a.exe src/*.c "./entry/$ENTRY" -Wall -g -lm
# fi
