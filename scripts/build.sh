#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
TARGET_DIR="$DIR/../out/lx"
while [[ $# -gt 0 ]]; do
	case $1 in
	-c|--compiler)
		CXX=$(which $2)
		TARGET_DIR="$DIR/../out/$2"
		shift
		shift
		;;
	-t|--test)
		TEST=YES
		shift
		;;
	-d|--docs)
		DOCS=YES
		shift
		;;
	--clean)
		rm -rf $TARGET_DIR
		shift
		;;
	*)
		echo "Unknown argument $1"
		exit 1
		;;
	esac
done

mkdir -p $TARGET_DIR
pushd $TARGET_DIR > /dev/null
CXX=$CXX cmake ../..
if [ "$DOCS" == "YES" ]; then
	cmake --build . --target docs
else
	cmake --build . --parallel 8
fi

if [ "$TEST" == "YES" ]; then
	ctest . --output-on-failure
fi
popd > /dev/null
