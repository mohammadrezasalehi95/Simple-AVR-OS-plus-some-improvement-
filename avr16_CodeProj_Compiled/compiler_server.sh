#!/bin/sh
SERVER_PORT=4444
nc -k -l $SERVER_PORT > watcher.out &
while true; do
    inotifywait watcher.out
    git pull origin master
    echo salam
    echo make | wine cmd
    now=$(date +"%T")
    last_line=$(tail -1 watcher.out)
    echo ${last_line}
    git add .
    git commit -m "compiled  [time: $now]"
    git push origin master
done