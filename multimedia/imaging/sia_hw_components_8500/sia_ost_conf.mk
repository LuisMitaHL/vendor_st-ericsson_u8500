# vim:syntax=make:
# -*- Mode: Makefile -*-
#
OST_COMPILER = 1

MODULE_NAME = sia_hw_components_8500
OST_MODULE_ID = 0x4C
# Translate value to hexa one for tracecompiler
OST_MODULE_ID_INT = `printf "%d\n" ${OST_MODULE_ID}`

OST_COMPONENT_NAME = SIA_HW_COMPONENTS_8500
OST_DICO_NAME = $(OST_COMPONENT_NAME)
OST_COMPONENT_INC = $(COMPONENT_TOP_DIR)/inc
#Add support for fixed trace id group
#MY_OST_COMPILER_OPTION = "--tgdf=$(MM_TOOLS_DIR)/examples/$(MODULE_NAME)/fixed_id.txt"
OST_SRC_DIR = adder dec_arm src
#TRACECOMPILERDIR = $(MM_TOOLS_DIR)/osttc

# List of all UID used. In this example we increment by 1  
OST_COMPONENT_ID_NMF_DSP = $(shell expr ${OST_MODULE_ID_INT} + 3 )
OST_COMPONENT_ID_NMF_ARM = $(shell expr ${OST_MODULE_ID_INT} + 2 )
OST_COMPONENT_ID_HOST = $(shell expr ${OST_MODULE_ID_INT} + 1 )
OST_COMPONENT_ID_TOP_1 = $(shell expr ${OST_MODULE_ID_INT} + 0)

#Top dictionary generation
OST_TOP_DICO_NAME_1 = $(OST_COMPONENT_NAME)



