-> sudo apt install libiberty-dev
-> getlibboost 1.61, isntall from sources
-> get dyninst last release, configure like this:
BOOST_ROOT="/home/pierre/Desktop/hermitux/boost_1_61_0/prefix" \
	cmake -DENABLE_STATIC_LIBS=1 \
	-DCMAKE_INSTALL_PREFIX=$PWD/../prefix ..
-> compile dyninst and install in some local prefix
-> edit Makefile in src to point to dyninst install prefix
