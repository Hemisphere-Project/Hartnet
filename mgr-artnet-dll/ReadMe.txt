## MinGW / MSYS ##

Installer le compiler MinGW:
http://www.mingw.org/wiki/Getting_Started
- telecharger et lancer mingw-get-setup.exe
- une fois executé, dans le gestionnaire:
		installer mingw-developer-toolkit / mingw32-base / mingw32-gcc-g++ / msys-base


Lancer MSYS
Télécharger mgr-artnet-dll
Décompresser dans C:\MinGW\msys\1.0\home\user

	cd mgr-artnet-dll-master/libartnet
	./configure --prefix=/home/mgr/mgr-artnet-dll-master/libartnet
	make
	make install
	cd ..
	./compile_mgr.sh

=> la dll est produite dans le dossier /home/mgr/mgr-artnet-dll-master/dll







## Libartnet ##

Télécharger libartnet
https://github.com/OpenLightingProject/libartnet

Le décompresser dans C:\MinGW\msys\1.0\home\mgr

Lancer MSYS et compiler libartnet:
	cd libartnet-master
	autoreconf -fi
	./configure ac_cv_func_malloc_0_nonnull=yes  ac_cv_func_realloc_0_nonnull=yes --host=i686-w64-mingw32  # pour x64: --host=x86_64-w64-mingw32
	# Retirer tous les Werror
	make
	make install

## mgr-artnet ##

Télécharger mgr-artnet-dll

Le décompresser dans C:\MinGW\msys\1.0\home\mgr

Lancer MSYS et compiler libartnet:
	cd mgr-artnet-dll-master/libartnet
	./configure ac_cv_func_malloc_0_nonnull=yes  ac_cv_func_realloc_0_nonnull=yes --host=i686-w64-mingw32  # pour x64: --host=x86_64-w64-mingw32
	# Retirer tous les Werror
	make
	make install

Lancert MSYS et compiler mgr-artnet-dll
	cd mgr-artnet-dll-master
	./configure_mgr.sh

=>  mgr-artnet-r512.dll est compilé :)
