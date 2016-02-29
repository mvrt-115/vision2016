#! /bin/sh
if g++ startPing.cpp udp_client_server.cpp -o startPing; then
    ./startPing
fi
