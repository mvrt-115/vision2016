#! /bin/bash

# Compile library
if g++ main.cpp build/lazer-vision.so -o vision `pkg-config opencv --cflags --libs` --std=c++11; then

	if [ $# -gt 1 ]; then
		./vision $1 $2
	else
		# images/ directory default RGB and Depth testing images
		./vision images/rgb.png images/depth.png 
	fi
fi
