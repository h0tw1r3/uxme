#!/bin/sh

. ${0%/*}/common

IFS=$'\r\n'

echo -e "# Changelog $(format_version $(git_tag_clean)) #"

RANGE="$(git_tag_previous)..HEAD"

echo -e "\n## UXME commits ##"

for author in $(git --no-pager log --first-parent --no-merges --pretty=format:"%an" ${RANGE} | sort | uniq -c) ;
do
	name=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\2/')
	count=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\1/')
	echo -e "\n### $name ($count commits)\n"
	git --no-pager log --author="$name" --date=short --no-merges --pretty=format:"* %s" ${RANGE} | grep -s -v "(nw)\| NW\| nw" || true
done

RANGE="$(git_tag_prev_mame_commit)..$(git_tag_mame_commit)"

echo -e "\n## MAME commits ##"

for author in $(git --no-pager log --no-merges --pretty=format:"%an" ${RANGE} | sort | uniq -c) ;
do
	name=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\2/')
	count=$(echo $author | sed 's/^\ *\([0-9]*\)\ \(.*\)$/\1/')
	echo -e "\n### $name ($count commits)\n"
	git --no-pager log --author="$name" --date=short --no-merges --pretty=format:"* %s" ${RANGE} | grep -s -v "(nw)\| NW\| nw" || true
done
