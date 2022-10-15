#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
mkdir -p $DIR/../out/lx
pushd $DIR/../out/lx
cmake ../..
cmake --build . --parallel 8

if [ "$1" == "test" ]; then
	ctest . --output-on-failure
fi
popd
