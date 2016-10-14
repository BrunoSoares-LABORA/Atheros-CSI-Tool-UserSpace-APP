#!/bin/bash
for i in `seq 1 $1`;
do
    ./recv_csi $3_$i &
    pid=$(pgrep recv_csi)
    echo $pid
    sleep $2
    kill -9 $pid
    sleep 3
done

