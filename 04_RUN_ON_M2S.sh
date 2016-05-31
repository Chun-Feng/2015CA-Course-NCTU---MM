#!/bin/bash
if [ ! -d ./00_Configuration ]; then
    echo "You are not in the Lab directory. Please change dir to it."
    exit
fi
if [ -f mm ] && [ -f matrix_mult.bin ]; then
m2s --x86-sim detailed\
    --x86-config ./00_Configuration/cpu-config.ini\
    --x86-report ./00_Report/cpu_report.ini\
    --si-sim detailed\
	--si-config ./00_Configuration/gpu-config.ini\
	--si-report ./00_Report/gpu_report.ini\
	--mem-config ./00_Configuration/mem-config.ini\
	--mem-report ./00_Report/mem_report.ini\
    ./mm\
    --load matrix_mult.bin
else
    echo "File doesn't exist. Please compile first."
	exit
fi

