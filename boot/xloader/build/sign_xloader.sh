#!/bin/sh

SIGNROOT= ${BUILD_ROOT}/
export LOCAL_SIGNPACKAGEROOT=${SIGNROOT}/packages
export LOCAL_KEYROOT=${SIGNROOT}/keys

export SIGNPACKAGE=U8500_STE_R1E
if [ $# -ne 2 ]; then
    echo "usage: sign_xloader.sh <bin-name> <ssw-name>" 1>&2
    exit 1
fi

echo "Signing xloader w sign-package $SIGNPACKAGE "$1" "$2""

${SHELL} ${SIGNROOT}/sign-tool-cli/sign-tool-u8500.sh \
    --local --verbose --sw-type xloader -f \
    --sign-package $SIGNPACKAGE \
    "$1" "$2"

