#!/bin/sh

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

APPNAME=uxme
BUILD=$1
ARCH=$2
COMPILER=mingw-gcc
EXE=.exe

if [ $ARCH = x64 ]; then
    APPNAME="${APPNAME}64"
fi
if [ $BUILD = Debug ]; then
    APPNAME="${APPNAME}d"
fi

BIN=../build/$COMPILER/bin/$ARCH/$BUILD
PKG=../build/$COMPILER/pkg/$ARCH/$BUILD

if [ ! -d "$BIN" ]; then
    decho "Invalid options, cant find the release directory"
    exit 1
fi
if [ -d "$PKG" ]; then
    decho "Clean $PKG"
    rm -rf "$PKG"
fi
decho "Create $PKG"
mkdir -p "$PKG"/tools

cp "$BIN/${APPNAME}${EXE}" "$PKG"/
cp "$BIN/${APPNAME}.sym"   "$PKG"/
cp "$BIN/chdman${EXE}"     "$PKG"/tools/
cp "$BIN/ldverify${EXE}"   "$PKG"/tools/
cp "$BIN/ldresample${EXE}" "$PKG"/tools/
cp "$BIN/romcmp${EXE}"     "$PKG"/tools/
cp "$BIN/jedutil${EXE}"    "$PKG"/tools/
cp "$BIN/ledutil${EXE}"    "$PKG"/tools/
cp "$BIN/unidasm${EXE}"    "$PKG"/tools/
cp "$BIN/nltool${EXE}"     "$PKG"/tools/
cp "$BIN/castool${EXE}"    "$PKG"/tools/
cp "$BIN/floptool${EXE}"   "$PKG"/tools/
cp "$BIN/imgtool${EXE}"    "$PKG"/tools/

cp -r ../docs "$PKG"/
cp -r ../hash "$PKG"/
cp -r ../hlsl "$PKG"/
cp -r ../samples "$PKG"/
cp -r ../artwork "$PKG"/
cp -r resources/dirs/* "$PKG"/
./changelog.sh > "$PKG"/CHANGELOG.md

VERSION=$(format_version $(git_tag_clean))

pushd "$PKG"
if [ ! -d dat ]; then
	mkdir dat
fi
curl -s -L https://build.zaplabs.com/project/uxme/faststart.dat > dat/faststart.dat

cd ..
ln -fs "$BUILD" "${APPNAME}-${VERSION}"
7za a -mpass=4 -mfb=255 -y -tzip -l "${BUILD}.zip" "${APPNAME}-${VERSION}"
unlink "${APPNAME}-${VERSION}"
popd >/dev/null
