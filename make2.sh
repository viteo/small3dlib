#!/bin/bash

PROGRAM=test

clear; clear; g++ -x c -g -fmax-errors=5 -pedantic -Wall -Wextra -o $PROGRAM $PROGRAM.c -lSDL2 2>&1 >/dev/null && ./$PROGRAM
