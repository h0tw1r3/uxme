#!/usr/bin/env bash

# BEGIN: common script bootstrap
SCRIPT_PATH="${BASH_SOURCE[0]}";
if ([ -h "${SCRIPT_PATH}" ]) then
	while([ -h "${SCRIPT_PATH}" ]) do SCRIPT_PATH=`readlink "${SCRIPT_PATH}"`; done
fi
pushd . > /dev/null
cd `dirname ${SCRIPT_PATH}` > /dev/null
SCRIPT_PATH=`pwd`;
popd > /dev/null
cd "$SCRIPT_PATH/.."
. ./common
# END: common script bootstrap

if [ ! -d ../build/osx_clang/bin/x32/Release ]; then
	decho "No x32 Release build"
	exit 1
fi
if [ ! -d ../build/osx_clang/bin/x64/Release ]; then
	decho "No x64 Release build"
	exit 1
fi

pushd ../build/osx_clang/bin/x32/Release
FILES=$(find . -maxdepth 1 -type f -executable)

if [ -d ../../universal ]; then
	rm -rf ../../universal
fi
mkdir -p ../../universal/Release

for FILE in $FILES
do
	FILE64=$FILE
	if [ ! -f ../../x64/Release/$FILE ]; then
		if [ ! -f ../../x64/Release/${FILE}64 ]; then
			echo "Unable to locate 64bit binary: $FILE"
			exit 1
		else
			FILE64=${FILE}64
		fi
	fi
	x86_64-apple-darwin11-lipo -create $FILE ../../x64/Release/$FILE64 -output ../../universal/Release/$FILE
done

popd
