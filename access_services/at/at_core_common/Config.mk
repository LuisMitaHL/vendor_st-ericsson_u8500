#
# Copyright (C) ST-Ericsson SA 2010
#

ifeq ($(ATC_ENABLE_FEATURE_CN),true)
CN := $(ACCESS_SERVICES_PATH)/call_network/call_network
EXE_USE_CN_SERVICE := true
$(info AT-Core: C&N enabled)
else
$(info AT-Core: C&N disabled)
endif

ifeq ($(ATC_ENABLE_FEATURE_AUDIO),true)
AUDIO := $(MULTIMEDIA_PATH)/audio/adm
EXE_USE_AUDIO_SERVICE := true

ifeq ($(ATC_ENABLE_FEATURE_AUDIO_LEGACY),true)
EXE_USE_AUDIO_SERVICE_LEGACY := true
endif
$(info AT-Core: AUDIO enabled)
else
$(info AT-Core: AUDIO disabled)
endif

ifeq ($(ATC_ENABLE_FEATURE_COPS),true)
COPS := $(PROCESSING_PATH)/security_framework/cops
EXE_USE_COPS_SERVICE := true
$(info AT-Core: COPS enabled)
else
$(info AT-Core: COPS disabled)
endif

ifeq ($(ATC_ENABLE_FEATURE_PSCC),true)
PSCC := $(CONNECTIVITY_PATH)
EXE_USE_PSCC_SERVICE := true
$(info AT-Core: PSCC enabled)
else
$(info AT-Core: PSCC disabled)
endif

ifeq ($(ATC_ENABLE_FEATURE_SIM),true)
SIM := $(ACCESS_SERVICES_PATH)/sim/sim
EXE_USE_SIM_SERVICE := true
$(info AT-Core: SIM enabled)
else
$(info AT-Core: SIM disabled)
endif

ifeq ($(ATC_ENABLE_FEATURE_SIMPB),true)
SIMPB := $(ACCESS_SERVICES_PATH)/sim/sim_phonebook
EXE_USE_SIMPB_SERVICE := true
$(info AT-Core: SIMPB enabled)
else
$(info AT-Core: SIMPB disabled)
endif


ifeq ($(ATC_ENABLE_FEATURE_SMS),true)
SMS := $(ACCESS_SERVICES_PATH)/sms/sms
EXE_USE_SMS_SERVICE := true
$(info AT-Core: SMS enabled)
else
$(info AT-Core: SMS disabled)
endif

ifeq ($(ATC_ENABLE_FEATURE_CSPSA),true)
CSPSA := $(STORAGE_PATH)/parameter_storage/cspsa
EXE_USE_CSPSA_SERVICE := true
$(info AT-Core: CSPSA enabled)
else
$(info AT-Core: CSPSA disabled)
endif

ifeq ($(ATC_ENABLE_FEATURE_ATC_CUSTOMER_EXTENSIONS),true)
EXE_EXTENSION_PATH := $(ACCESS_SERVICES_PATH)/at/at_core_extension/internal/executor
CONFIG_EXTENSION_PATH := $(ACCESS_SERVICES_PATH)/at/at_core_extension/config
PARSER_EXTENSION_PATH := $(ACCESS_SERVICES_PATH)/at/at_core_extension/internal/parser
EXE_USE_ATC_CUSTOMER_EXTENSIONS := true
include $(ACCESS_SERVICES_PATH)/at/at_core_extension/inc_Config.mk
endif

