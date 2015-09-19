#!/bin/bash

cd "${0%/*}"

SUPPORT_PATH="${HOME}/Library/Application Support/uxme"
DEFAULT_FONT="PixelOperator"

if [ ! -d "${SUPPORT_PATH}" ]; then
	cp -r ../Resources/defaults "${SUPPORT_PATH}"
	chmod -R ug+w "${SUPPORT_PATH}"
	./%%TARGETNAME%% -createconfig -uifont ${DEFAULT_FONT}
fi

# FIXME: ATSApplicationFontsPath doesn't seem to work
if [ ! -f "${HOME}/Library/Fonts/${DEFAULT_FONT}.ttf" ]; then
	cp ../Resources/Fonts/${DEFAULT_FONT}.ttf "${HOME}"/Library/Fonts/
	chmod ug+w "${HOME}"/Library/Fonts/PixelOperator.ttf
fi

./%%TARGETNAME%%
