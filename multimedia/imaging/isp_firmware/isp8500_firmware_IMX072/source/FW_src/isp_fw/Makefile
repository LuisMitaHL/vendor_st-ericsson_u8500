
include $(MM_MAKEFILES_DIR)/SharedDefines.mk

OST_COMPILER = 1
OST_MODULE_ID = 0x59

# Translate value to hexa one for tracecompiler
OST_MODULE_ID_INT = `printf "%d\n" ${OST_MODULE_ID}`
OST_COMPONENT_NAME := $(subst /, ,$(COMPONENT_TOP_DIR))
OST_COMPONENT_NAME := $(lastword $(OST_COMPONENT_NAME))

# List of all UID used. In this example we increment by 1
OST_COMPONENT_ID = $(shell expr ${OST_MODULE_ID_INT} + 0)
OST_COMPONENT_ID_TOP_1 = $(shell expr ${OST_MODULE_ID_INT} + 1)

# Top dictionary generation
OST_TOP_DICO_NAME_1 = $(OST_COMPONENT_NAME)

OST_DICO_NAME = $(OST_COMPONENT_NAME)
OST_SRC_DIR = PictorProject
MEDIUM_DIR = $(OST_SRC_DIR)
INSTALL_DICO = $(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/SharedTraceCompiler.mk

all: $(DICO_INSTALL)

# [PM].PHONY: all is already declared in SharedRules.mk

# [PM]Required to trigger the clean of generic makefiles, to delete xml generated.
#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
