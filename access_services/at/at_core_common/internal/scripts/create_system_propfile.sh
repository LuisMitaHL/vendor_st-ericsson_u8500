#!/bin/sh

if [ "$#" -ne 3 ]
then
    echo "Incorrect amount of inparameters. Usage:"
    echo ""
    echo "`basename $0` directoryname manufacturer model"
    exit 1
fi

SYSTEM_SW_ID_FILE="$1/system_id.cfg"
SYSTEM_MANUF_ID_FILE="$1/manuf_id.cfg"
SYSTEM_MODEL_ID_FILE="$1/model_id.cfg"

GIT_DIR="${ANDROID_BUILD_TOP}/.repo/manifests/.git"
COMMIT="$(git --git-dir=${GIT_DIR} log --pretty=format:"%H" -1)"

# Find tag for current commit
TAG="$(git --git-dir=${GIT_DIR} describe --tags ${COMMIT})"

if [ "$TAG" = "" ]
then
    TAG="Not available"
    echo "WARNING! Could not find a software-version tag for current commit-id." >&2
fi

echo -n "$TAG" > "$SYSTEM_SW_ID_FILE"

echo -n "$2" > "$SYSTEM_MANUF_ID_FILE"

echo -n "$3" > "$SYSTEM_MODEL_ID_FILE"
