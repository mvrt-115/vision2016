#! /bin/sh
if g++ stopPing.cpp udp_client_server.cpp -o stopPing; then
    ./stopPing
fi
