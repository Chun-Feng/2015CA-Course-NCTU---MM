#!/bin/bash
if [ ! -d ./00_Configuration ]; then
    echo "You are not in the Lab directory. Please change dir to it."
    exit
fi
if [ -d ./00_Report ]; then
    rm -rf ./00_Report;
	mkdir 00_Report
fi
