#!/bin/bash
for i in `seq 1 $1`;
do
    ./send_Data wlan0 28:cf:da:f3:db:33 $2
done
