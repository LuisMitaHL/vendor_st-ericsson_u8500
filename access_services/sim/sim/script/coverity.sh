#!/bin/sh

### Coverity script for sim/sat ###
#
# Usage example:
#  ~/bin/coverity.sh leakage_test_commit_1391062c87
#
#

# Abort if no version tag is found
if [ "$1" = "" ];
then
  echo "Please specify a version tag! (first argument)"
  return
fi

# Start Coverity
/proj/fairbanks_lx/tools/coverity/ste-prevent --database=TELE --product=u8500 --part=android --clean_cov_data --no_model --version_tag=$1 \
  '(source /proj/fairbanks_lx/etc/settings_for_bash_classic; \
    source $TOP/build/envsetup.sh; \
    setpaths; \
    pushd $TOP/vendor/st-ericsson/access_services/sim/sim; \
    mm -B; \
    popd
  )'

# Display information on how to view results
echo ""
echo "Please login to https://seldx080.lud.stericsson.com:1759 to be able to see the results!"