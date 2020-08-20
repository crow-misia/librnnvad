#!/usr/bin/env bash

export LC_ALL=C

CURDIR=$(cd $(dirname $0)/..; pwd)
SRCREPO="$1"

set -ueo pipefail

if [ $# -ne 1 ]; then
  echo "Usage: $0 PATH"
  echo "PATH is a local clone of WebRTC repository."
  exit 1
fi

if [ ! -d "$SRCREPO" ]; then
  echo "$SRCREPO is not a directory"
  exit 1
fi

cd $CURDIR

PATHS=$(grep -e '^[^#]' tools/import-paths) || exit 1

OLDCOMMIT=$(head -n 1 tools/import-commit) || exit 1
NEWCOMMIT=$(cd "$SRCREPO"; git log --format='%H' -1 -- $PATHS) || exit 1

if [ "$OLDCOMMIT" = "$NEWCOMMIT" ]; then
  echo "Nothing to do, no changes since $OLDCOMMIT."
  exit
fi

echo "Importing changes from $OLDCOMMIT to $NEWCOMMIT...".

(cd "$SRCREPO"; git format-patch -M -B --stdout "$OLDCOMMIT..$NEWCOMMIT" -- $PATHS) | \
  git am --whitespace=fix --directory=import || exit 1

echo "$NEWCOMMIT" > tools/import-commit || exit 1

echo "Done. You should commit the changed tools/import-commit file."

