
MY_DIR = $(shell pwd -P)
DIRS_TUNING := $(patsubst $(MY_DIR)/%,%, $(foreach dir,$(MY_DIR),$(wildcard $(dir)/tuning_*)))
	
DIRECTORIES=$(DIRS_TUNING) 

include $(MM_MAKEFILES_DIR)/SharedDispatch.mk
