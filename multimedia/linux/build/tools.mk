#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

# insure that some minimal libs are in the path
LD_LIBRARY_PATH:=/lib:/usr/lib:/usr/local/lib:/lib32:/usr/lib32:/usr/X11R6/lib:$(LD_LIBRARY_PATH)

################################################################################
# Support for MMDSP compilation
################################################################################
export DSPTOOLS=$(TOOLS_PATH)/host/mmdsp
PATH:=$(DSPTOOLS)/bin/Linux:$(PATH)
export MCPROJECT=$(DSPTOOLS)/target/project/nomadik/STn8500/audio
LD_LIBRARY_PATH:=$(LD_LIBRARY_PATH):$(DSPTOOLS)/lib/Linux

################################################################################
# Support for ARM RVCT compilation
################################################################################
# This is handled by $(TOOLS_PATH)/host/rvct/envsetup.sh

################################################################################
# Support for XP70 SIA COMPILATION
################################################################################

################################################################################
# From STxP70.sh
SX:=$(TOOLS_PATH)/host/xp70_sia
export SX

# Needed only when building from STWorkench
#STWORKBENCH:=$(SX)/stworkbench
#export STWORKBENCH
#STJRE:=$(SX)/jre
#export STJRE

################################################################################
# From STxP70_environment.sh
SXARCHITECTURE:=stxp70v3
export SXARCHITECTURE

################################################################################
# From toolset_version.sh
SXVER:=4.0.0
export SXVER

################################################################################
# From compiler_version.sh
STXP70CC_VERSION:=4.2
export STXP70CC_VERSION

################################################################################
# From STxP70_addon_environment.sh
# Normally not needed since only used when building extensions
# Assumption is that XP70 extensions are already build if used in the toolset we are providing in the XP70 toolset GIT
#PATH:=$(SX)/corxpert/stxp70v3/bin:${PATH}

################################################################################
# From STxP70_environment.sh
# Normally not needed since only used when building extensions
# Assumption is that XP70 extensions are already build if used in the toolset we are providing in the XP70 toolset GIT
#PATH:=$(STJRE)/bin:$(PATH)
#PATH:=$(SX)/gnu/4.1.2/i686-pc-linux-gnu/bin:$(PATH)
#PATH:=$(SX)/gnu/4.1.2/bin:$(PATH)
PATH:=$(PATH):$(SX)/bin
PATH:=$(PATH):$(SX)/stxp70cc/$(STXP70CC_VERSION)/bin

# Normally not needed since only used when building extensions
# Assumption is that XP70 extensions are already build if used in the toolset we are providing in the XP70 toolset GIT
# LD_LIBRARY_PATH:=$(SX)/reconftoolkit/lib:$(LD_LIBRARY_PATH)
#LD_LIBRARY_PATH:=$(SX)/reconftoolkit/cmodel/host/lib:$(LD_LIBRARY_PATH)

LD_LIBRARY_PATH:=$(LD_LIBRARY_PATH):$(SX)/bin
LD_LIBRARY_PATH:=$(LD_LIBRARY_PATH):$(SX)/stxp70cc/${STXP70CC_VERSION}/lib/cmplrs

