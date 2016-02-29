#! /bin/sh
if g++ ping.cpp udp_client_server.cpp -o ping; then
    ./ping
fi
