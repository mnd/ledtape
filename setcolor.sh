#!/bin/bash
stty -F /dev/ttyUSB0 9600 raw

echo $1 > /dev/ttyUSB0
echo $2 > /dev/ttyUSB0
echo $3 > /dev/ttyUSB0
