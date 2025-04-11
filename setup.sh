#!/bin/bash
trap 'echo "# $BASH_COMMAND"' DEBUG

# set comiller
export CC="$(which clang)"
export CXX="$(which clang++)"


CMAKE_CONFIG_ARGS=""


if [[ $* == *--update* ]]; then 
	CMAKE_CONFIG_ARGS="--fresh"
	git pull origin main
	
fi

# Configure project
cmake -B build ${CMAKE_CONFIG_ARGS} -DCMAKE_BUILD_TYPE=Release .
cd build
cmake --build . -j $(nproc) --config Release

if [[ $* == *--run* ]]; then
	./logmat
fi
