#!/bin/bash

# error handling setup
set -o nounset
set -o pipefail
set -o errtrace

error() {
	echo "ERROR in $0 : line $1 exit code $2"
	exit $2
}
trap 'error ${LINENO} ${?}' ERR

SCRATCH=$(mktemp -d -t package.XXXXXXXX)

SCRIPT_PATH="${BASH_SOURCE[0]}";
if ([ -h "${SCRIPT_PATH}" ]) then
	while([ -h "${SCRIPT_PATH}" ]) do SCRIPT_PATH=`readlink "${SCRIPT_PATH}"`; done
fi
pushd . > /dev/null
cd `dirname ${SCRIPT_PATH}` > /dev/null
SCRIPT_PATH=`pwd`;

function finish {
	[ -d $SCRATCH ] && rm -rf "$SCRATCH"
}
trap finish EXIT

TARGETNAME=$1
BUILDTYPE=$2
ARCH=$3
BUILDTOOL=osx_clang
SUFFIX=

if [ $ARCH = x64 ]; then
    TARGETNAME="${TARGETNAME}64"
fi
if [ $BUILDTYPE = Debug ]; then
    TARGETNAME="${TARGETNAME}d"
fi

BIN=../../build/$BUILDTOOL/bin/$ARCH/$BUILDTYPE
APPCONTENT="${SCRATCH}/disk/${TARGETNAME}.app/Contents"

LASTTAG=$(git describe --tag --abbrev=0)
VERSION=$(git describe --tag | sed 's/-[^-]*$//')

# short version converts 0.165-10 to 1650010
# or 1.3-1 to 100030001
arrVERSION=(${VERSION//[.-]/ })
NUMVERSION=""
for VERPART in ${arrVERSION[@]} ;
do
	[ $VERPART -gt 0 ] && NUMVERSION="${NUMVERSION}$(printf '%04d' ${VERPART})"
done
NUMVERSION=$(expr $NUMVERSION + 0)

mkdir -p "${APPCONTENT}"/{MacOS,Resources,Resources/defaults}

# add version to plist
cat Info.plist | sed 's/%%POINTVERSION%%/'${VERSION//-/.}'/' | \
	sed 's/%%SHORTVERSION%%/'${NUMVERSION}'/' > "${APPCONTENT}"/Info.plist

cp -a application.icns "${APPCONTENT}"/Resources/
cp -a launch.sh "${APPCONTENT}"/MacOS/
cp -a "${BIN}/${TARGETNAME}${SUFFIX}" "${APPCONTENT}"/MacOS/

../changelog.sh ${LASTTAG} | pandoc -f markdown -t html5 -o "${SCRATCH}"/disk/CHANGELOG.html
pandoc -f markdown_github -t html5 ../../README.md -o "${SCRATCH}"/disk/README.html

# support files
mkdir "${SCRATCH}"/disk/tools
TOOLS="chdman ldverify ldresample romcmp jedutil unidasm nltool castool floptool imgtool aueffectutil"
for TOOL in $TOOLS ; do
	cp "${BIN}"/${TOOL}${SUFFIX} "${SCRATCH}"/disk/tools/
done

# documentation
pandoc -f markdown -t html5 ../resources/"MAME Legal Information.md" -o "${SCRATCH}"/disk/"MAME Legal Information.html"
cp -r ../../docs "${SCRATCH}"/disk/Documentation
cp -r ../../{hash,hlsl,web,nl_examples,samples,artwork} "${APPCONTENT}"/Resources/defaults
cp -r ../resources/dirs/* "${APPCONTENT}"/Resources/defaults/
cp -r ../resources/fonts "${APPCONTENT}"/Resources/Fonts

genisoimage -D -V "${TARGETNAME} ${VERSION}" -no-pad -r -apple -o ${SCRATCH}/uncompressed.dmg ${SCRATCH}/disk/
dmg dmg ${SCRATCH}/uncompressed.dmg ../../uxme.dmg
