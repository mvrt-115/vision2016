# Compile library
if g++ calib.cpp build/lazer-vision.so -o vision -L/usr/local/cuda-6.5/lib/ `pkg-config opencv --cflags --libs` --std=c++11; then
	./vision > images/calib-fps.png
fi
