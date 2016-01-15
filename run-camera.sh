# Compile library
if g++ main.cpp build/lazer-vision.so -o vision `pkg-config opencv --cflags --libs` --std=c++11; then
	./vision 1 	# Dummy value to force program to run camera
fi
