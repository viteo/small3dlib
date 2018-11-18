#!/bin/bash

clear; clear

make

if [ $? -eq 0 ]; then
  ./PokittoEmu ./BUILD/firmware.bin $1
fi
