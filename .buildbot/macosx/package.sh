#!/bin/bash

# error handling setup
set -o nounset -o pipefail -o errtrace

error() {
	local lc="$BASH_COMMAND" rc=$?
	echo "ERROR in $0 : line $1 exit code $2"
	echo "      [$lc]"
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
	#[ -d $SCRATCH ] && rm -rf "$SCRATCH"
	echo $SCRATCH
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
REVCOUNT=$(git log ${LASTTAG}..HEAD --oneline | wc -l)
BUNDLEVERSION=${VERSION//[v-]/.}; BUNDLEVERSION=${BUNDLEVERSION#"."}
SHORTVERSION=${LASTTAG//v/}
echo "LASTTAG: ${LASTTAG}"
echo "VERSION: ${VERSION}"
echo "REVCOUNT: ${REVCOUNT}"
echo "BUNDLEVERSION: ${BUNDLEVERSION}"
echo "SHORTVERSION: ${SHORTVERSION}"

mkdir -p "${APPCONTENT}"/{MacOS,Resources,Resources/defaults}

# bundle
echo "Bundle..."
cat Info.plist | sed 's/%%BUNDLENAME%%/'${TARGETNAME}'/' | sed 's/%%SHORTVERSION%%/'${SHORTVERSION}'/' | sed 's/%%BUNDLEVERSION%%/'${BUNDLEVERSION}'/' > "${APPCONTENT}"/Info.plist
if [ -f ${TARGETNAME}.icns ]; then
	cp ${TARGETNAME}.icns "${APPCONTENT}"/Resources/application.icns
else
	cp application.icns "${APPCONTENT}"/Resources/
fi
cat launch.sh | sed 's/%%TARGETNAME%%/'${TARGETNAME}'/' > "${APPCONTENT}"/MacOS/launch.sh
chmod +x "${APPCONTENT}"/MacOS/launch.sh
cp -a "${BIN}/${TARGETNAME}${SUFFIX}" "${APPCONTENT}"/MacOS/

# support files
echo "Tools..."
mkdir "${SCRATCH}"/disk/tools
TOOLS="chdman ldverify ldresample romcmp jedutil unidasm nltool castool floptool imgtool aueffectutil"
for TOOL in $TOOLS ; do
	cp "${BIN}"/${TOOL}${SUFFIX} "${SCRATCH}"/disk/tools/
done

# documentation
echo "Documentation..."
pandoc -f markdown_github -t html5 ../../README.md -o "${SCRATCH}"/disk/README.html
pandoc -f markdown -t html5 ../resources/"MAME Legal Information.md" -o "${SCRATCH}"/disk/"Legal Information.html"
cp -r ../../docs "${SCRATCH}"/disk/Docs
../changelog.sh ${LASTTAG} | pandoc -f markdown -t html5 -o "${SCRATCH}"/disk/Docs/CHANGELOG.html

# defaults
echo "Defaults..."
cp -r ../../{hash,hlsl,nl_examples,samples,artwork} "${APPCONTENT}"/Resources/defaults
cp -r ../resources/dirs/* "${APPCONTENT}"/Resources/defaults/
cp -r ../resources/fonts "${APPCONTENT}"/Resources/Fonts

# dmg
echo "Dmg..."
#cp DS_Store "${SCRATCH}/disk/.DS_Store"
cp VolumeIcon.icns "${SCRATCH}/disk/.VolumeIcon.icns"
#mkdir "${SCRATCH}/disk/.background"
#cp background.png "${SCRATCH}/disk/.background/background.png"

WORKDMG="${SCRATCH}/uncompressed.dmg"
SCRATCHSZ=$(du -s "${SCRATCH}" | cut -f1)
dd if=/dev/zero of="${WORKDMG}" bs=1112 count=${SCRATCHSZ}
mkfs.hfsplus -v "${TARGETNAME}" "${WORKDMG}"
hfsplus "${WORKDMG}" addall "${SCRATCH}/disk/"
hfsplus "${WORKDMG}" attr / C
#hfsplus "${WORKDMG}" symlink " " /Applications
# TODO: genisoimage has drawbacks, but most people dont have libdmg-hfsplus built
#       make conditional package based on best available tools.
# genisoimage -D -V "AttractMode ${VERSION#v}" -no-pad -r -apple -o ${SCRATCH}/uncompressed.dmg ${SCRATCH}/disk/
dmg dmg "${WORKDMG}" ../../${TARGETNAME}.dmg
