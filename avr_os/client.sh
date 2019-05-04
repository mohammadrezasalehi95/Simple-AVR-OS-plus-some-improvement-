git pull origin master
git add .
git commit -m $1
git push origin master
sleep 2s
echo fire | telnet 192.168.8.100 4444

