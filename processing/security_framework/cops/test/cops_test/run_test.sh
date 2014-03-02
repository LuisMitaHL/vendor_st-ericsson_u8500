COPS_ROOT=${PWD}/../..
INDICATION_DIR=`mktemp -d --tmpdir cops_indication_dir_XXXX`

env LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${COPS_ROOT}/cops-api" \
    COPSD=$COPS_ROOT/copsd/copsd \
    TAPP_TEST=$COPS_ROOT/tapp_test \
    TEMPLATES_DIR=$COPS_ROOT/test/cops_test \
    GEN_SIPC=$COPS_ROOT/test/gen_sipc/gen_sipc \
    SDATA=$COPS_ROOT/test/gen_sipc/sdata \
    KEYS=$COPS_ROOT/test/gen_sipc/keys/ \
    INDICATION_DIR=$INDICATION_DIR \
    ./cops_test

if [ -n "$INDICATION_DIR" ]; then
	rm -r $INDICATION_DIR
fi
