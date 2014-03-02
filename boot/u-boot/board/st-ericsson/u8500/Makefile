#
# Copyright (C) ST-Ericsson SA 2009
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#


include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(BOARD).a

# We want warnings free code.
# Some generic include files have inline functions with unused parameters.
CFLAGS	+= -Wall -Wextra -Werror -Wno-unused-parameter
ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
CFLAGS += -DENG_VERSION
endif

# Please keep one file per line and in alphabetical order
ifeq ($(ENABLE_FEATURE_BUILD_HBTS),)
COBJS	+= ab8500vibra.o
COBJS	+= cmd_cdump.o
COBJS	+= cmd_hats.o
endif
COBJS	+= u8500.o
COBJS	+= u8500_i2c.o
COBJS	+= cmd_ab8500.o
COBJS	+= cmd_rbtreason.o

COBJS-$(CONFIG_RECOVERY_MODE) += cmd_recovery.o
COBJS-$(CONFIG_ANDROID_BCB) += cmd_bcb.o
COBJS-$(CONFIG_CMD_BOOTI) += cmd_booti.o
COBJS-$(CONFIG_U8500_KEYPAD) += u8500_keypad.o

ifeq ($(ENABLE_FEATURE_BUILD_HBTS),true)
COBJS	+= cmd_mmc_init.o
COBJS	+= cmd_version.o
endif

ifeq ($(ENABLE_FEATURE_BUILD_HBTS),)
COBJS-$(CONFIG_VIDEO_LOGO) += mcde_display.o
COBJS-$(CONFIG_VIDEO_LOGO) += mcde_display_dpi.o
COBJS-$(CONFIG_VIDEO_LOGO) += mcde_display_dsi.o
COBJS-$(CONFIG_VIDEO_LOGO) += mcde_display_image.o
COBJS-$(CONFIG_VIDEO_LOGO) += mcde_hw.o
endif

COBJS	+= $(COBJS-y)

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

$(LIB): $(obj).depend $(OBJS) $(SOBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) $(SOBJS)

clean:
	rm -f $(SOBJS) $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
