#!/bin/bash
trap 'echo "# $BASH_COMMAND"' DEBUG

# set comiller
export CC="$(which clang)"
export CXX="$(which clang++)"


CMAKE_CONFIG_ARGS=""
CMAKE_BUILD_ARGS=""

if [ -d "$1" ]; then
	cd $1
fi

if [[ $* == *--update* ]]; then 
	CMAKE_CONFIG_ARGS="--fresh"
	CMAKE_BUILD_ARGS="--clean-first"

	git fetch --all
	git reset --hard origin/main
	git clean
	git pull origin main
fi

 

# Configure project
cmake -B build ${CMAKE_CONFIG_ARGS} -DCMAKE_BUILD_TYPE=Release .
cd ./build
cmake --build . ${CMAKE_BUILD_ARGS} -j $(nproc) --config Release

if [[ $* == *--run* ]]; then
	./logmat
fi
