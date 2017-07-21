## MinGW / MSYS ##

Installer le compiler MinGW:
http://www.mingw.org/wiki/Getting_Started
- telecharger et lancer mingw-get-setup.exe
- une fois executé, dans le gestionnaire:
		installer mingw-developer-toolkit / mingw32-base / mingw32-gcc-g++ / msys-base


Lancer MSYS
Télécharger Hartnet-master.zip
https://github.com/Hemisphere-Project/Hartnet

Décompresser dans C:\MinGW\msys\1.0\home\user

	cd Hartnet-master/libartnet
	./configure --prefix=/home/mgr/Hartnet-master/libartnet  #remplacer mgr par le user
	make
	make install
	cd ..
	#remplacer mgr par le user dans compile_mgr.sh
	./compile_mgr.sh

=> la dll est produite dans le dossier /home/mgr/Hartnet-master/dll


