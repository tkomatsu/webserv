#!/bin/bash
WEBSERV_ROOT=$PWD

echo "             _ _   _            _   "
echo " _   _ _ __ (_) |_| |_ ___  ___| |_ "
echo "| | | | '_ \| | __| __/ _ \/ __| __|"
echo "| |_| | | | | | |_| ||  __/\__ \ |_ "
echo " \__,_|_| |_|_|\__|\__\___||___/\__|"                         

cmake -S . -B build
cmake --build build
cd build && ctest
if [ $? -ne 0 ]; then
	./WebservTest
fi

echo " _     _   _         _            _   "
echo "| |__ | |_| |_ _ __ | |_ ___  ___| |_ "
echo "| '_ \| __| __| '_ \| __/ _ \/ __| __|"
echo "| | | | |_| |_| |_) | ||  __/\__ \ |_ "
echo "|_| |_|\__|\__| .__/ \__\___||___/\__|"
echo "              |_|                     "

cd ../../
make re
./webserv &
sleep 1
python3 -m pip install requests
python3 ./test/http/main.py

if [ $? -eq 0 ]; then
    pkill webserv
    exit 0
else
	pkill webserv
    exit 1
fi
