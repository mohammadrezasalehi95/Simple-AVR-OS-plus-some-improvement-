git add .
git commit -m $1
git push origin master
sleep 2s
/bbecho fire | telnet 192.168.8.100 4444

