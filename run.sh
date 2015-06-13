#! /bin/bash

# Compile library
g++ main.cpp build/lazer-vision.so -o vision `pkg-config opencv --cflags --libs` --std=c++11 -I/usr/local/include/libfreenect -lfreenect -lfreenect_sync

# Run executable using fakenect
fakenect thanksgiving0 ./vision


# Run executable using fakenect, but loop it
# while :
# do
# 	echo $(fakenect fakenect-testing/thanksgiving0 ./vision) > run.log
# 
# 	# Search log for appropriate ending to fakenect
# 	if grep -Fxq "$FILENAME" run.log
# 	then
#    		kill 1 # Interrupt Signal
# 	fi	
# 
# 	sleep 1 # Pause for 1 second
# 
# done
