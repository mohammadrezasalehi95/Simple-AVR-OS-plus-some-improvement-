#!/bin/sh
SERVER_IP=192.168.1.12
SERVER_PORT=4444
echo fire | telnet ${SERVER_IP} ${SERVER_PORT}
git add .
git commit -m $1