#!/bin/sh

if [ -z "$TMPDIR" ]; then
    TMPDIR=/tmp
fi

sudo apt-get install -y \
    cmake gcc gcc-c++ gcc-c++-common \
    libjsoncpp24 jsoncpp-devel libuuid-devel zlib \
    zlib-devel libssl-devel libsqlite3 libsqlite3-devel


if [ -e /usr/local/lib64/libdrogon.a ]; then
    echo "[!SETUP-ENV] DROGON IS INSTALLED"
else
    git clone https://github.com/drogonframework/drogon.git $TMPDIR/drogon
    cd $TMPDIR/drogon
    git submodule update --init
    cmake -B build .
    cd ./build
    cmake --build . -j $(nproc)
    sudo cmake --install .
fi
