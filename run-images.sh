#! /bin/bash

# Compile library
if g++ main.cpp build/lazer-vision.so -o vision `pkg-config opencv --cflags --libs` --std=c++11 -I/usr/local/include/libfreenect -I~/Documents/boost_1_58_0 -L~/Documents/boost_1_58_0/stage/lib -lfreenect -lfreenect_sync -lboost_system -lboost_filesystem; then

	# let i=1
	# FILES=images/*

	# for file in FILES # List all files in images/ folder
	# do
	# 	echo "(" i ") $file"
	# done

	# read -u 3 input # Read input from user
	# 
	# if [[input -eq 0]]
	# do
	# 	./vision 

	if [ $# -gt 1 ]; then
		./vision $1 $2
	else
		# images/ directory default RGB and Depth testing images
		./vision images/rgb.png images/depth.png 
	fi
fi
