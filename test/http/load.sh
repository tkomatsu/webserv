#!/bin/bash
# ./webservしながら使う

methods=("GET" "POST")

for ((i=0; i<1000; i++));
do
  # sleep 0.01
  PORT=`expr 4200 + $RANDOM % 3`
  curl localhost:$PORT -X ${methods[$RANDOM % 2]}
done