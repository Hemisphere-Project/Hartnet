#### compile_win32.bat
rm dll/mgr-artnet-r512.dll

#### SET THESE DIRECTORIES as needed !
#INC_ARTNET=./libartnet/
INC_ARTNET=/home/mgr/Hartnet-master/libartnet/include
#LIB_ARTNET=/usr/local/lib
LIB_ARTNET=/home/mgr/Hartnet-master/libartnet/lib

echo '=== Compiling mgr-artnet-r512.dll'
g++ -Wall -I $INC_ARTNET ./mgr-artnet-r512.c -shared -o dll/mgr-artnet-r512.dll \
    -L $LIB_ARTNET -lartnet -lwsock32 -lnetapi32 -liphlpapi -static-libgcc -static-libstdc++

echo 'finished compiling'
