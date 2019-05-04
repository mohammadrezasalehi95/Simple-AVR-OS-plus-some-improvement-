nc -k -l 4444 > watcher.out &
while true; do
    inotifywait watcher.out
    git pull origin master
    mkdir build.err
    mkdir build.dep
    mkdir build.lst
    mkdir build.obj
    mkdir build.rom
    echo salam
#    echo make clean| wine cmd
    echo make | wine cmd
    now=$(date +"%T")
    last_line=$(tail -1 watcher.out)
    echo ${last_line}
    git add .
    git commit -m "compiled  [time: $now]"
    git push origin master
done