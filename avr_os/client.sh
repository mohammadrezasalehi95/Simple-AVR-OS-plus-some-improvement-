git pull origin master
git add .
git commit -m $1
git push origin master
sleep 2s
echo fire   fire |telnet 192.168.8.100 4444
echo fire|telnet 192.168.8.100 4444
echo fire|telnet 192.168.8.100 4444
echo fire|telnet 192.168.8.100 4444
sleep 2s
echo ended
while true; do
    sleep 10s
    git pull origin master
done
