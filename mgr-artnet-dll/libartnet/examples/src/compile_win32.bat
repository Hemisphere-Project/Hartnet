#### compile_win32.bat
rm artnet-discover.exe
rm artnet-recv.exe
rm artnet-setdmx.exe
rm example_exe.exe
rm example_dll.dll

#### SET THESE DIRECTORIES as needed !
INC_ARTNET=/C/Projekte/libartnet-win32/Develope/libartnet/
LIB_ARTNET=/usr/local/lib

#### eventually needed for examples with tvision
INC_TVISION=/C/Projekte/TEST/tvision/include
LIB_TVISION=/C/Projekte/TEST/tvision/makes

####echo '=== Compiling libartnet-win32 examples'
####echo ' Include path ' $INC_ARTNET ' and ' $INC_TVISION
####echo ' Library path ' $LIB_ARTNET ' and ' $LIB_TVISION
####echo ' YOU MIGHT NEED TO CHANGE THESE PATHS '
####echo

####echo 'compiling artnet-discover ...'
####g++ -Wall -I $INC_ARTNET \
####     ./artnet-discover.c -o artnet-discover.exe \
####    -L $LIB_ARTNET -lartnet -lwsock32 -lnetapi32 -liphlpapi

####echo 'compiling artnet-recv ...'
####g++ -Wall -I $INC_ARTNET \
####		./artnet-recv.c -o artnet-recv.exe \
####    -L $LIB_ARTNET -lartnet -lwsock32 -lnetapi32 -liphlpapi

####echo 'compiling artnet-setdmx ...'
####g++ -Wall -I $INC_ARTNET \
####		./artnet-setdmx.c -o artnet-setdmx.exe \
####    -L $LIB_ARTNET -lartnet -lwsock32 -lnetapi32 -liphlpapi

echo '=== Compiling mgr-artnet-r512.dll'
g++ -Wall -I $INC_ARTNET \
	./mgr-artnet-r512.c -shared -o mgr-artnet-r512.dll \
    -L $LIB_ARTNET -lartnet -lwsock32 -lnetapi32 -liphlpapi




echo 'finished compiling'
