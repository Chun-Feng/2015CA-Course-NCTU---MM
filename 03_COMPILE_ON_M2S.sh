#!/bin/bash
if [ ! -d ./00_Configuration ]; then
    echo "You are not in the Lab directory. Please change dir to it."
    exit
fi
if [ -f ./mm ]; then
    rm mm
fi
gcc -std=c99 matrix_mult.c -o mm -I/opt/multi2sim-4.2/runtime/include/ -L/opt/multi2sim-4.2/lib/.libs/ -lm2s-opencl -m32
m2c --amd --amd-device 11 matrix_mult.cl
