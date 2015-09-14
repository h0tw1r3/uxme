#!/bin/sh

VERSION=$1
IFS=$'\r\n'

echo -e "# Changelog $VERSION #\n"
echo -e "## Post release commits ##"

RANGE="${VERSION}..HEAD"
for author in $(git --no-pager log --simplify-merges --pretty=format:"%an" ${RANGE} | sort | uniq -c) ;
do
	name=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\2/')
	count=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\1/')
	echo -e "\n### $name ($count commits)\n"
	git --no-pager log --author="$name" --date=short --simplify-merges --pretty=format:"* %s" ${RANGE} | grep -v "(nw)\| NW\| nw"
done

MAMEVERSION="mame$(echo $VERSION | sed 's/[^0-9]//g')"
PREVMAMEVERSION=$(git describe --tag --abbrev=0 ${MAMEVERSION}^)
RANGE="${PREVMAMEVERSION}..${MAMEVERSION}"

echo -e "\n## Commits"

for author in $(git --no-pager log --simplify-merges --pretty=format:"%an" ${RANGE} | sort | uniq -c) ;
do
	name=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\2/')
	count=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\1/')
	echo -e "\n### $name ($count commits)\n"
	git --no-pager log --author="$name" --date=short --simplify-merges --pretty=format:"* %s" ${RANGE} | grep -v "(nw)\| NW\| nw"
done
