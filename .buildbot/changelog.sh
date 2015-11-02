#!/bin/sh

VERSION=$1
IFS=$'\r\n'

echo -e "# Changelog $VERSION #"

RANGE="${VERSION}..HEAD"
i=0
for author in $(git --no-pager log --no-merges --pretty=format:"%an" ${RANGE} | sort | uniq -c) ;
do
	[ $i -eq 0 ] && echo -e "\n## Commits made after ${TAG} ##"
	name=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\2/')
	count=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\1/')
	echo -e "\n### $name ($count commits)\n"
	git --no-pager log --author="$name" --date=short --no-merges --pretty=format:"* %s" ${RANGE} | grep -s -v "(nw)\| NW\| nw" || true
	(( i++ ))
done

MAMEVERSION="mame$(echo $VERSION | sed 's/[^0-9]//g')"
PREVMAMEVERSION=$(git describe --tag --abbrev=0 ${MAMEVERSION}^)
RANGE="${PREVMAMEVERSION}..${MAMEVERSION}"

echo -e "\n## Commits"

for author in $(git --no-pager log --no-merges --pretty=format:"%an" ${RANGE} | sort | uniq -c) ;
do
	name=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\2/')
	count=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\1/')
	echo -e "\n### $name ($count commits)\n"
	git --no-pager log --author="$name" --date=short --no-merges --pretty=format:"* %s" ${RANGE} | grep -s -v "(nw)\| NW\| nw" || true
done
