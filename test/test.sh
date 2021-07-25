#!/bin/bash

cmake -S . -B build
cmake --build build
cd build && ctest
if [ $? -ne 0 ]; then
	./WebservTest
fi
