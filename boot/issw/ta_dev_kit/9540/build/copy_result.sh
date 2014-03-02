#!/bin/bash
set -e

# Copy the files to the modules where they should be used

if [ -z ${DEST_DIR} ]; then
    echo "no destination directory set. No copy is done"
else
    if [ -d ${DEST_DIR} ]; then
        if [ -z ${SKIP_COPY_SSW} ]; then
            cp -frv *$BINARY*.ssw $DEST_DIR
        fi
        if [ -z ${SKIP_COPY_BIN} ]; then
            cp -frv $BINARY.bin $DEST_DIR
        fi
    else
        echo $DEST_DIR "does not exist in this tree. No copy of the ta is done"
    fi
fi
