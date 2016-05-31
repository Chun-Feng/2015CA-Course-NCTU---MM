#!/bin/bash
if [ ! -d ./00_Configuration ]; then
    echo "You are not in the Lab directory. Please change dir to it."
    exit
fi
if [ -f ./mm ]; then
    rm mm
fi
gcc -std=c99 -Wall -DUNIX -g -DDEBUG -m64 -o mm matrix_mult.c -I/opt/AMDAPPSDK-2.9-1/include/ -L/opt/AMDAPPSDK-2.9-1/lib/x86_64 -lOpenCL
m2c --amd --amd-device 26 matrix_mult.cl
