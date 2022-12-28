#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
mkdir -p $DIR/../out/clang
pushd $DIR/../out/clang
CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake ../..
cmake --build . --parallel 8

if [ "$1" == "test" ]; then
	ctest . --output-on-failure
fi
popd
