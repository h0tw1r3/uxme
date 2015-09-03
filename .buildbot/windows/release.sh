#!/bin/sh

# switch to the buildbot directory
SCRIPT_PATH="${BASH_SOURCE[0]}";
if ([ -h "${SCRIPT_PATH}" ]) then
  while([ -h "${SCRIPT_PATH}" ]) do SCRIPT_PATH=`readlink "${SCRIPT_PATH}"`; done
fi
pushd . > /dev/null
cd `dirname ${SCRIPT_PATH}` > /dev/null
SCRIPT_PATH=`pwd`;
popd  > /dev/null
cd "$SCRIPT_PATH/.."

APPNAME=$1
BUILD=$2
ARCH=$3
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
    echo "Invalid options, cant find the release directory"
    exit 1
fi
if [ -d "$PKG" ]; then
    echo "Clean $PKG"
    rm -rf "$PKG"
fi
echo "Create $PKG"
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
cp -r ../web "$PKG"/
cp -r ../nl_examples "$PKG"/
cp -r ../samples "$PKG"/
cp -r ../artwork "$PKG"/
cp -r resources/dirs/* "$PKG"/

GITVERSION=$(git describe --tag --abbrev=0)
./changelog.sh $GITVERSION > "$PKG"/CHANGELOG.md

VERSION=$(grep '^#define BARE_BUILD_VERSION' ../src/version.c | sed 's/.*"\(.*\)"$/\1/')

pushd "$PKG" >/dev/null
cd ..
ln -s "$BUILD" "${APPNAME}-${VERSION}"
zip -9 -r "${BUILD}.zip" "${APPNAME}-${VERSION}"
unlink "${APPNAME}-${VERSION}"
popd >/dev/null
