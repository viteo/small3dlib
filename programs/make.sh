#!/bin/bash

PROGRAM=testTerminal

clear; clear; g++ -x c -g -fmax-errors=5 -pedantic -O3 -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter -Wno-missing-field-initializers -o $PROGRAM $PROGRAM.c -lSDL2 2>&1 >/dev/null && ./$PROGRAM
#clear; clear; clang -x c -g -pedantic -O3 -Wall -Wextra -o $PROGRAM $PROGRAM.c -lSDL2 2>&1 >/dev/null && ./$PROGRAM
