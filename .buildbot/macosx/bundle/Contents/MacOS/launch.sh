#!/bin/bash

cd "${0%/*}"

SUPPORT_PATH="$HOME/Library/Application Support/uxme"

if [ ! -d "$HOME/Library/Application Support/uxme" ]; then
	mkdir "$SUPPORT_PATH"
fi

./uxme
