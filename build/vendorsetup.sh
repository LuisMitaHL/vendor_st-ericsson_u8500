# Copyright (C) ST-Ericsson SA 2011
# License terms: BSD.

#
# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-eng

# Lunch menu

T=$(gettop)
if [ -e $T/vendor/st-ericsson/u8500/BoardConfig.mk ] || [ -e $T/device/st-ericsson/u8500/BoardConfig.mk ]; then
  add_lunch_combo ste_u8500-eng
  add_lunch_combo ste_u8500-user
  add_lunch_combo ste_u8500_hats-eng
  add_lunch_combo ste_u8500_hbts-eng
fi
if [ -e $T/vendor/st-ericsson/u5500/BoardConfig.mk ] || [ -e $T/device/st-ericsson/u5500/BoardConfig.mk ]; then
  add_lunch_combo ste_u5500-eng
  add_lunch_combo ste_u5500-user
fi
if [ -e $T/vendor/st-ericsson/u9500_21/BoardConfig.mk ] || [ -e $T/device/st-ericsson/u9500_21/BoardConfig.mk ]; then
  add_lunch_combo ste_u9500_21-eng
  add_lunch_combo ste_u9500_21_hats-eng
fi
if [ -e $T/vendor/st-ericsson/u9500_100/BoardConfig.mk ] || [ -e $T/device/st-ericsson/u9500_100/BoardConfig.mk ]; then
  add_lunch_combo ste_u9500_100-eng
  add_lunch_combo ste_u9500_100_hats-eng
fi
if [ -e $T/vendor/st-ericsson/u9540/BoardConfig.mk ] || [ -e $T/device/st-ericsson/u9540/BoardConfig.mk ]; then
  add_lunch_combo ste_u9540-eng
  add_lunch_combo ste_u9540_hats-eng
  add_lunch_combo ste_u9540_hbts-eng
fi
if [ -e $T/vendor/st-ericsson/l8540/BoardConfig.mk ] || [ -e $T/device/st-ericsson/l8540/BoardConfig.mk ]; then
  add_lunch_combo ste_l8540-eng
  add_lunch_combo ste_l8540_hats-eng
  add_lunch_combo ste_l8540_hbts-eng
fi

# Verify that all required tools and packages to build the ST-Ericsson platform are installed on host.
if [ -e $T/vendor/st-ericsson/tools/host/verify_environment/check_tools_versions/check-tools-versions.py ]; then
  echo ""
  echo "Verifying platform development build environment"
  python $T/vendor/st-ericsson/tools/host/verify_environment/check_tools_versions/check-tools-versions.py
fi

# Setup the RVCT environment and check that there is a valid license
if [ -e $T/vendor/st-ericsson/tools/host/rvct/envsetup.sh ]; then
  echo "Setup RVCT environment"
  . $T/vendor/st-ericsson/tools/host/rvct/envsetup.sh
fi

# Get some neat commands for building multimedia.
if [ -e $T/vendor/st-ericsson/multimedia/linux/build/mmbuild.sh ]; then
  echo "including vendor/st-ericsson/multimedia/linux/build/mmbuild.sh"
  source $T/vendor/st-ericsson/multimedia/linux/build/mmbuild.sh
fi
