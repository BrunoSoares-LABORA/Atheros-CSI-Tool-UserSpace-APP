#!/bin/bash
for i in `seq 1 $1`;
do
    ./send_Data wlan0 7c:c3:a1:b4:03:df $2
done
