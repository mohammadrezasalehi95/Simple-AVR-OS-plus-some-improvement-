git pull origin master
git add src
git commit -m $1
git push origin master
timeout 5 netcat salamsalam.hopto.org 4444<client.sh
sleep 2s
echo ended
while true; do
    sleep 10s
    git pull origin master
done
