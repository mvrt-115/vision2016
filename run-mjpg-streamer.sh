#! /bin/bash

# Run the mjpg_streamer for port at localhost:8080 using the mjpeg file in ./images/
mjpg_streamer -i "input_file.so -f ./images/" -o "output_http.so -w /usr/local/www"
