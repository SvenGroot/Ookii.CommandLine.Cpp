#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
mkdir -p $DIR/../out/lx
pushd $DIR/../out/lx
cmake ../..
cmake --build . --target docs
popd
