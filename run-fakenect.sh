#! /bin/bash

# Compile library
if g++ main.cpp build/lazer-vision.so -o vision `pkg-config opencv --cflags --libs` --std=c++11 -I/usr/local/include/libfreenect -I~/Documents/boost_1_58_0 -L~/Documents/boost_1_58_0/stage/lib -lfreenect -lfreenect_sync -lboost_system -lboost_filesystem; then
    # Run executable using fakenect
    fakenect thanksgiving0 ./vision
fi
