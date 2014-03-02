# -*- Mode: Makefile -*-
# vim:syntax=make:

## The MMDSP code is board specific
## MMDSP_CODE_IS_BOARD_SPECIFIC:=true

ifndef COMPONENT_TOP_DIR
  COMPONENT_TOP_DIR := ..
endif

#OST TRACE SPECIFIC
include ../make.ost_conf

# 537072982 is the ID reseverd for audiocodec nmf/mpc
OST_SRC_DIR       = dmain dmaout capturercontroller renderercontroller dmaout_bt dmain_bt \
		    msp_dma_controller msp_dma_ctrl_ab8500 msp_dma_ctrl_bt
OST_COMPONENT_ID  = $(KUidDicAudiocodecNmfMpc)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_mpc
#CPPFLAGS+=-I../inc -I$(OST_COMPONENT_INC)/autogentraces
INSTALL_DICO=$(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/AudioNmfConfig.mk

$(info "COMPONENT_TOP_DIR = $(COMPONENT_TOP_DIR)")

SRCPATH +=$(COMPONENT_TOP_DIR)/../.. $(COMPONENT_TOP_DIR)/mpc/include $(COMPONENT_TOP_DIR)/inc $(OST_COMPONENT_INC)/autogentraces

ITF_PREFIX = audiocodec

#CPPFLAGS+=-Iinclude -DNVERBOSE

USER2MPCITFS_CPP= \
	dmaout.low_power \
	common.configure \
	msp_dma_ctrl_ab8500.configure

MPC2USERITFS_CPP= \
	dmaout.low_signal \
	msp_dma_ctrl_ab8500.bitclock

COMPONENTS= \
	audiocodec.mpc.host_reg \
	audiocodec.mpc.msp_dma_controller \
	audiocodec.mpc.dmaout \
	audiocodec.mpc.dmain \
	audiocodec.mpc.renderercontroller \
	audiocodec.mpc.capturercontroller \
	audiocodec.mpc.dmaout_bt \
	audiocodec.mpc.msp_dma_ctrl_bt \
	audiocodec.mpc.msp_dma_ctrl_ab8500 \
	audiocodec.mpc.dmain_bt


ASYNCITFS=common.portstate

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk


#TESTLISTS = testlist.lst
#DATABASE  = mpc
#TEST_TAG  = $(MMDSPPROCESSOR)

#include $(MM_MAKEFILES_DIR)/AudioTest.mk

test cleantest:
	@echo No test to run!

