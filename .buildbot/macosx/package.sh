#!/bin/bash

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

SCRATCH=$(mktemp -d -t package.XXXXXXXX)

function finish {
	sync
	[ -d $SCRATCH ] && rm -rf "$SCRATCH"
}
trap finish EXIT

TARGETNAME=uxme
BUILDTYPE=$1
ARCH=$2
BUILDTOOL=osx_clang
SUFFIX=

if [ $ARCH = x64 ]; then
    TARGETNAME="${TARGETNAME}64"
fi
if [ $BUILDTYPE = Debug ]; then
    TARGETNAME="${TARGETNAME}d"
fi

BIN=../build/$BUILDTOOL/bin/$ARCH/$BUILDTYPE
APPCONTENT="${SCRATCH}/disk/${TARGETNAME}.app/Contents"

BUNDLEVERSION=$(format_version $(git_tag_clean))
SHORTVERSION=$(format_version $(git_tag_bare))
decho "BUNDLEVERSION: ${BUNDLEVERSION}"
decho "SHORTVERSION: ${SHORTVERSION}"

mkdir -p "${APPCONTENT}"/{MacOS,Resources,Resources/defaults}

# bundle
decho "Bundle..."
cat macosx/Info.plist | sed 's/%%BUNDLENAME%%/'${TARGETNAME}'/' | sed 's/%%SHORTVERSION%%/'${SHORTVERSION}'/' | sed 's/%%BUNDLEVERSION%%/'${BUNDLEVERSION}'/' > "${APPCONTENT}"/Info.plist
if [ -f ${TARGETNAME}.icns ]; then
	cp macosx/${TARGETNAME}.icns "${APPCONTENT}"/Resources/application.icns
else
	cp macosx/application.icns "${APPCONTENT}"/Resources/
fi
cat macosx/launch.sh | sed 's/%%TARGETNAME%%/'${TARGETNAME}'/' > "${APPCONTENT}"/MacOS/launch.sh
chmod +x "${APPCONTENT}"/MacOS/launch.sh
cp -a "${BIN}/${TARGETNAME}${SUFFIX}" "${APPCONTENT}"/MacOS/

# support files
decho "Tools..."
mkdir "${SCRATCH}"/disk/tools
TOOLS="chdman ldverify ldresample romcmp jedutil unidasm nltool castool floptool imgtool aueffectutil"
for TOOL in $TOOLS ; do
	cp "${BIN}"/${TOOL}${SUFFIX} "${SCRATCH}"/disk/tools/
done

# documentation
decho "Documentation..."
pandoc -f markdown_github -t html5 ../README.md -o "${SCRATCH}"/disk/README.html
pandoc -f markdown -t html5 resources/"MAME Legal Information.md" -o "${SCRATCH}"/disk/"Legal Information.html"
cp -r ../docs "${SCRATCH}"/disk/Docs
./changelog.sh | pandoc -f markdown -t html5 -o "${SCRATCH}"/disk/Docs/CHANGELOG.html

# defaults
decho "Defaults..."
cp -r ../{hash,hlsl,nl_examples,samples,artwork,bgfx,shaders,plugins,language} "${APPCONTENT}"/Resources/defaults
cp -r resources/dirs/* "${APPCONTENT}"/Resources/defaults/
cp -r resources/fonts "${APPCONTENT}"/Resources/Fonts
if [ ! -d "${APPCONTENT}"/Resources/defaults/dat ]; then
	mkdir "${APPCONTENT}"/Resources/defaults/dat
fi
curl -s -L https://build.zaplabs.com/project/uxme/faststart.dat > "${APPCONTENT}"/Resources/defaults/dat/faststart.dat

# dmg
decho "Dmg..."
#cp DS_Store "${SCRATCH}/disk/.DS_Store"
cp macosx/VolumeIcon.icns "${SCRATCH}/disk/.VolumeIcon.icns"
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
dmg dmg "${WORKDMG}" ../${TARGETNAME}.dmg
