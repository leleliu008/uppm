#!/bin/sh

set -ex

for item in `git log --format=short | grep 'commit [0-9a-z]\{40\}' | cut -c8-`
do
    if git show "$item" | grep sqlite ; then
        echo "$item"
    fi
done
