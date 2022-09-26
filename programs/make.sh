#!/bin/sh

# Make script for small3dlib programs.
# by drummyfish
# released under CC0 1.0, public domain

if [ "$#" -ne 1 ]; then
  echo "ERROR: expecting one argument, the name of program without extension (e.g. \"helloWorld\")"
  exit 0
fi

link=""

clear
clear

flags="-x c -g -lm -O3 -std=c99 -pedantic -Wall -Wextra -o $1 $1.c -lSDL2"
compiler=gcc
#compiler=clang

echo "making:" ${compiler} ${flags}

${compiler} ${flags} && ./$1
