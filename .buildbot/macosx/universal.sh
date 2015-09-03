#!/usr/bin/env bash

if [ ! -d build/osx_clang/bin/x32/Release ]; then
	echo "No x32 Release build"
	exit 1
fi
if [ ! -d build/osx_clang/bin/x64/Release ]; then
	echo "No x64 Release build"
	exit 1
fi

pushd build/osx_clang/bin/x32/Release
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
