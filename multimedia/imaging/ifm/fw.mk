
MY_DIR = $(shell pwd -P)
DIRS_FW := $(patsubst $(MY_DIR)/%,%, $(foreach dir,$(MY_DIR),$(wildcard $(dir)/isp8500*)))

DIRECTORIES=$(DIRS_FW) 
	

include $(MM_MAKEFILES_DIR)/SharedDispatch.mk
